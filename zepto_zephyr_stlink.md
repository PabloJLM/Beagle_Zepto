# BeagleConnect Zepto con Zephyr y ST-Link

Guía probada en Ubuntu 24 con un ST-Link V2 (firmware V2J46S7) y la placa BeagleConnect Zepto (MSPM0L1117). 

Resultado final, el blinky corre y el LED azul D3 (PA13) parpadea.

La guía oficial de Zephyr solo documenta el flasheo con XDS110 o por UART. 

*El ST-Link por OpenOCD es un camino no oficial, por eso hay que compilar un OpenOCD reciente.*

---
 
## Antes de cada sesión, activar el entorno virtual
 
Cada terminal nueva necesita el entorno virtual activo, o el comando `west` no se encuentra.
 
```
source ~/zephyrproject/.venv/bin/activate
```
 
Sabrás que quedó activo cuando el prompt empiece con `(.venv)`. Para salir de él se escribe `deactivate`.
 
---

---

## 1. Cableado del ST-Link a J3

| ST-Link | J3 de la Zepto |
|---|---|
| 3.3V | pin 1 (VCC) |
| SWDIO | pin 2 |
| SWCLK | pin 3 |
| GND | pin 5 |
| NRST | pin 7 (opcional) |


---

## 2. Instalar Zephyr

### 2.1 Dependencias del sistema

```
sudo apt update
sudo apt install --no-install-recommends git cmake ninja-build gperf ccache dfu-util device-tree-compiler wget python3-dev python3-venv python3-tk xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1
```

### 2.2 Entorno virtual y west

```
mkdir -p ~/zephyrproject
python3 -m venv ~/zephyrproject/.venv
source ~/zephyrproject/.venv/bin/activate
pip install west
```

### 2.3 Descargar Zephyr

```
west init -m https://github.com/zephyrproject-rtos/zephyr ~/zephyrproject
cd ~/zephyrproject
west update
```

### 2.4 Dependencias de Python y registro

El orden importa. Las dependencias de Python van primero, porque `west zephyr-export` necesita el módulo `jsonschema`.

```
west packages pip --install
west zephyr-export
```

Alternativa si el primero falla.

```
pip install -r ~/zephyrproject/zephyr/scripts/requirements.txt
west zephyr-export
```

### 2.5 Toolchain ARM

```
cd ~/zephyrproject/zephyr
west sdk install --toolchains arm-zephyr-eabi
```

---

## 3. Compilar un OpenOCD reciente en tu carpeta

El OpenOCD de apt (0.12.0) no trae el driver de flash para MSPM0. Se compila uno desde git, instalado solo en tu carpeta personal.

### 3.1 Dependencias

```
sudo apt install -y build-essential git libtool autoconf automake pkg-config texinfo libusb-1.0-0-dev libjim-dev
```

### 3.2 Compilar e instalar

```
cd ~
git clone https://github.com/openocd-org/openocd.git
cd openocd
./bootstrap
./configure --enable-stlink --prefix=$HOME/openocd-local
make -j$(nproc)
make install
```

### 3.3 Agregar al PATH

```
echo 'export PATH=$HOME/openocd-local/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
openocd --version
```

Verifica que incluya el driver.

```
strings ~/openocd-local/bin/openocd | grep -i mspm0 | head
```

### 3.4 Permisos USB del ST-Link (si sale "open failed")

```
sudo tee /etc/udev/rules.d/60-stlink.rules > /dev/null <<'EOF'
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748|374b|374d|374e|374f|3752|3753", MODE="0666"
EOF
sudo udevadm control --reload-rules && sudo udevadm trigger
```

Desenchufa y vuelve a enchufar el ST-Link.

---

## 4. Compilar el blinky

Cada terminal nueva necesita el entorno virtual activo. Sabrás que está activo cuando el prompt empiece con `(.venv)`.

```
source ~/zephyrproject/.venv/bin/activate
cd ~/zephyrproject/zephyr
west build -p always -b beagleconnect_zepto/mspm0l1117 samples/basic/blinky
```

Resultado esperado, unos 13 KB de flash y 4 KB de RAM usados. El archivo queda en `build/zephyr/zephyr.elf`.

---

## 5. Probar la conexión

```
openocd -f interface/stlink.cfg -f target/ti/mspm0.cfg -c "adapter speed 1000" -c "init; halt; shutdown"
```

Debe aparecer `SWD DPIDR 0x6ba02477` y `Cortex-M0+ r0p1 processor detected`.

La conexión es intermitente. A veces falla con `init mode failed (unable to connect to the target)`. Es normal, solo repite el comando.

---

## 6. Flashear

Se usa un bucle que reintenta hasta que la conexión funcione.

```
cd ~/zephyrproject/zephyr
for i in 1 2 3 4 5 6; do
  echo "intento $i"
  openocd -f interface/stlink.cfg -f target/ti/mspm0.cfg -c "adapter speed 1000" -c "init; reset halt; program build/zephyr/zephyr.elf verify reset exit" > ocd.log 2>&1
  grep -q "Examination succeed" ocd.log && break
  sleep 1
done
tail -25 ocd.log
```

Salida esperada.

```
** Programming Finished **
** Verify Started **
Error: checksum mismatch - attempting binary compare
** Verified OK **
** Resetting Target **
```

El `checksum mismatch` es normal con este driver, lo importante es el `Verified OK`. El aviso `Unidentified PART[0xaf6c]` solo es cosmético, el driver lee el tamaño de flash directo del chip. Después de flashear, el LED azul D3 parpadea.

---

## 7. Problemas conocidos

| Síntoma | Causa y solución |
|---|---|
| `west` no se encuentra | Falta activar el entorno con `source ~/zephyrproject/.venv/bin/activate` |
| `No module named 'jsonschema'` | Faltó instalar las dependencias de Python antes de `west zephyr-export` |
| `source ... does not match the source ... used to generate cache` | Hay una carpeta `build` vieja. Bórrala con `rm -rf ~/zephyrproject/zephyr/build` |
| `libjim.so.0.79 not found` | El OpenOCD viejo de `/usr/local/bin` está roto. Usa el de `~/openocd-local` |
| `open failed` al abrir el ST-Link | Falta la regla udev o el ST-Link no está enchufado |
| `init mode failed (unable to connect to the target)` | Falla intermitente. Repite, usa el bucle, o presiona SW2 un segundo antes del comando |
| `Could not find MEM-AP to control the core` | No uses `connect_assert_srst`. Mantener el chip en reset apaga el debug de MSPM0 |
| `Invalid command argument` en `program` | La última palabra del `program` debe ser `exit`, no `shutdown`. Con `shutdown` lo toma como dirección |
| `west flash` falla | La placa fuerza XDS110 en su `board.cmake`. Con ST-Link se flashea con OpenOCD a mano |

---

## 8. Alternativa oficial por UART (BSL)

Si prefieres no depender de OpenOCD, la guía de Zephyr usa el bootloader por UART con `bb-imager-rs` y el archivo `build/zephyr/zephyr.bin`. Necesitas un adaptador USB a UART de 3.3 V.

- TX del adaptador a J3 pin 6 (BSL_UART_RX)
- RX del adaptador a J3 pin 4 (BSL_UART_TX)
- GND a pin 5 y 3.3 V a pin 1
- Pin 8 (BSL_INVOKE) a 3.3 V durante el reset para entrar al bootloader

---

## Referencias

- Página de la placa en Zephyr, `beagleconnect_zepto/mspm0l1117`
- Repositorio de la placa, github.com/beagleboard/beagleconnect-zepto
- OpenOCD, `tcl/target/ti/mspm0.cfg` y `src/flash/nor/mspm0.c`
