#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
[ -n "$1" ] || { echo "uso: $0 nombre_app"; exit 1; }
[ ! -e "$1" ] || { echo "'$1' ya existe"; exit 1; }
cp -r template "$1"
rm -rf "$1/build" "$1/README.md"
sed -i "s/APP_NAME/$1/" "$1/CMakeLists.txt"
echo "Creada $1/  ->  cd $1 && ./build.sh && ./flash.sh"
