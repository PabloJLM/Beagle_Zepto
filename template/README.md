# Plantilla Zephyr para la Zepto

Placa: `beagleconnect_zepto/mspm0l1117`

## Crear una app nueva
Desde la raiz del repo:

    ./new_app.sh mi_app

Crea `mi_app/` desde `template/` con el nombre del proyecto ya puesto.

## Uso
    ./build.sh          # compila (pasa args extra a west, ej. -- -DCONFIG_X=y)
    ./flash.sh          # flashea con ST-Link + OpenOCD

## Estructura
- `CMakeLists.txt`  proyecto (el nombre lo pone new_app.sh)
- `prj.conf`        Kconfig
- `src/main.c`      codigo (agrega mas .c en CMakeLists con target_sources)
- Overlay opcional: `boards/beagleconnect_zepto_mspm0l1117.overlay`
