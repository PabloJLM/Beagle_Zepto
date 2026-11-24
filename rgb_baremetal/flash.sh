#!/usr/bin/env bash
# Flashea build/rgb.elf a la Zepto con ST-Link + OpenOCD (reintenta si la conexion falla)
cd "$(dirname "$0")"
export PATH="$HOME/openocd-local/bin:$PATH"
for i in 1 2 3 4 5 6; do
  echo "intento $i"
  openocd -f interface/stlink.cfg -f target/ti/mspm0.cfg -c "adapter speed 1000" \
    -c "init; reset halt; program build/rgb.elf verify reset exit" > ocd.log 2>&1
  grep -q "Verified OK" ocd.log && { echo "Flasheado OK"; exit 0; }
  sleep 1
done
tail -25 ocd.log; echo "Fallo el flasheo"; exit 1
