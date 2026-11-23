# BeagleConnect Zepto con MSPM0L1117

Referencia de la variante con el MSPM0L1117SRHBR. Basado en variants.ods, en el esquematico zepto.kicad_sch (Rev Mark4, fecha 2026-02-15) y en los archivos de la placa en Zephyr xd

## El MCU

| Dato | Valor |
|---|---|
| Parte | MSPM0L1117SRHBR |
| Nucleo | Arm Cortex-M0+ a 32 MHz |
| Flash | 128 KB |
| RAM | 16 KB |
| Paquete | VQFN de 32 pines (RHB), 5 x 5 mm, paso de 0.5 mm |
| GPIO | 28 (PA0 a PA27) |
| Depuracion | SWD de 2 pines |
| Costo del chip en el ODS | 0.51 USD |
| Target en Zephyr | beagleconnect_zepto/mspm0l1117 |

Comparte huella con la MSPM0L1228 (256 KB) y la MSPM0G1519 (512 KB), por eso el esquematico menciona las tres.

## Pines (QFN32)

| Pin | Puerto | Funcion | Destino | Notas |
|---|---|---|---|---|
| 1 | PA0 | BSL_I2C_SDA | J5.3, Qwiic J1.3 y J2.3 | pull-up R5 de 5.1K a VCC |
| 2 | PA1 | BSL_I2C_SCL | J5.4, Qwiic J1.4 y J2.4 | pull-up R4 de 5.1K a VCC |
| 3 | NRST | RESET | J3.7, SW2 | R3 de 47K a +3V3, C2 de 10nF a GND |
| 4 | VDD | +3V3 | salida del ferrite FB1 | ver Alimentacion |
| 5 | VSS | GND | GND | |
| 6 | PA2 | ROSC | R6 de 100K a GND | opcional, 0.1% 25ppm |
| 7 | PA3 | SPI0_CS1 | J4.7 | |
| 8 | PA4 | SPI0_POCI | J4.9 | |
| 9 | PA5 | SPI0_PICO | J4.10 | |
| 10 | PA6 | SPI0_SCK | J4.8 | |
| 11 | PA7 | TIMA0_C1 | J5.8 | |
| 12 | PA8 | UART1_TX | J5.6 | |
| 13 | PA9 | UART1_RX | J5.5 | |
| 14 | PA10 | BSL_UART_TX | J3.4 | UART0 en Zephyr |
| 15 | PA11 | BSL_UART_RX | J3.6 | UART0 en Zephyr |
| 16 | PA12 | TIMG0_C0 | J5.9 | |
| 17 | PA13 | TIMG0_C1 | J5.10 y LED azul D2 | compartido, ver LEDs |
| 18 | PA14 | CLK_OUT | J5.11 | |
| 19 | PA15 | WS2812 | DIN del LED D3 | sin header, solo el LED |
| 20 | PA16 | FCC_IN | J5.2 | lectura de variante con R7, R8 y R9 |
| 21 | PA17 | TIMA0_C3 | J5.12 | |
| 22 | PA18 | BSL_INVOKE | SW1, J3.8 | boton de usuario y entrada al bootloader |
| 23 | PA19 | SWDIO | J3.2 | |
| 24 | PA20 | SWCLK | J3.3 | |
| 25 | PA21 | TIMG8_C0 | J4.1 | |
| 26 | PA22 | RST | J4.6 | GPIO, es el RST de mikroBUS, no el NRST del MCU |
| 27 | PA23 | INT | J5.7 | |
| 28 | PA24 | A0_3 | J4.5 | entrada analogica AN de mikroBUS |
| 29 | PA25 | A0_2 | J4.4 | |
| 30 | PA26 | A0_1 | J4.3 | |
| 31 | PA27 | A0_0 | J4.2 | |
| 32 | VCORE | VCORE | C6 de 1uF a GND | no conectar nada mas |
| 33 | EXP | GND | pad expuesto | pines 34 a 38 son TMPAD a GND |

## Alimentacion

- VCC entra por J3.1, J4.11 y el pin 2 de los conectores Qwiic.
- VCC pasa por el ferrite FB1 (FB-120R) y se convierte en +3V3, que alimenta VDD del MCU. No hay regulador en el esquematico, asi que VCC debe ser 3.3 V.
- Filtrado en VCC con C4 de 10uF. En +3V3 con C7 de 10uF y C8 de 100nF.
- El LED D3 se alimenta de VCC, no de +3V3.

## Conectores

### J3, depuracion y programacion (1x8)

| Pin | Senal | Puerto |
|---|---|---|
| 1 | VCC | |
| 2 | SWDIO | PA19 |
| 3 | SWCLK | PA20 |
| 4 | BSL_UART_TX | PA10 |
| 5 | GND | |
| 6 | BSL_UART_RX | PA11 |
| 7 | NRST | NRST |
| 8 | BSL_INVOKE | PA18 |

### J4, mikroBUS lado izquierdo (1x12)

| Pin | Senal | Puerto |
|---|---|---|
| 1 | TIMG8_C0 | PA21 |
| 2 | A0_0 | PA27 |
| 3 | A0_1 | PA26 |
| 4 | A0_2 | PA25 |
| 5 | A0_3 (AN) | PA24 |
| 6 | RST | PA22 |
| 7 | SPI0_CS1 | PA3 |
| 8 | SPI0_SCK | PA6 |
| 9 | SPI0_POCI | PA4 |
| 10 | SPI0_PICO | PA5 |
| 11 | VCC | |
| 12 | GND | |

### J5, mikroBUS lado derecho (1x12)

| Pin | Senal | Puerto |
|---|---|---|
| 1 | GND | |
| 2 | FCC_IN | PA16 |
| 3 | BSL_I2C_SDA | PA0 |
| 4 | BSL_I2C_SCL | PA1 |
| 5 | UART1_RX | PA9 |
| 6 | UART1_TX | PA8 |
| 7 | INT | PA23 |
| 8 | TIMA0_C1 | PA7 |
| 9 | TIMG0_C0 | PA12 |
| 10 | TIMG0_C1 | PA13 |
| 11 | CLK_OUT | PA14 |
| 12 | TIMA0_C3 | PA17 |

### J1 y J2, Qwiic (JST-SH de 4 pines)

| Pin | Senal |
|---|---|
| 1 | GND |
| 2 | VCC |
| 3 | SDA (PA0) |
| 4 | SCL (PA1) |

Los pull-ups de I2C son R4 y R5 de 5.1K. El esquematico indica que los pines I2C y UART del BSL son open drain tolerantes a 5 V y requieren pull-up.

## LEDs y botones

| Elemento | Conexion | Notas |
|---|---|---|
| D1 LED rojo | VCC, D1, R1 de 5.1K, GND | siempre encendido, no tiene GPIO |
| D2 LED azul | PA13 (TIMG0_C1), D2, R2 de 5.1K, GND | activo en alto, tambien sale por J5.10 |
| D3 ARGB-2020 | DIN en PA15, VDD en VCC, VSS en GND, C3 de 10uF | opcional, DOUT sin conectar |
| SW1 | entre PA18 (BSL_INVOKE) y +3V3 | boton de usuario, activo en alto |
| SW2 | entre NRST y GND | reset |

## En Zephyr

Datos tomados del archivo beagleconnect_zepto.dts.

| Elemento | Definicion |
|---|---|
| led0 | PA13, activo en alto |
| sw0 (button0) | PA18, el DTS lo declara activo en bajo |
| Consola | uart0 en PA10 (TX) y PA11 (RX) |
| ADC | referencia VDD, 12 bits, canales de PA27, PA26, PA25, PA24, PA18, PA22, PA12, PA14, PA16 y PA17 |
| RGB | no esta definido en el DTS, hay que agregarlo |

