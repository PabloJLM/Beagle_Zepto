#!/usr/bin/env bash
# Compila esta app para la Zepto. Funciona desde cualquier carpeta.
APP="$(cd "$(dirname "$0")" && pwd)"
source ~/zephyrproject/.venv/bin/activate
export ZEPHYR_BASE=~/zephyrproject/zephyr
cd ~/zephyrproject/zephyr || exit 1
west build -p auto -b beagleconnect_zepto/mspm0l1117 "$APP" -d "$APP/build" "$@"
