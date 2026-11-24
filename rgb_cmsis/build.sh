#!/usr/bin/env bash
# Compila el demo bare metal (no necesita west ni Zephyr, solo el compilador del SDK)
cd "$(dirname "$0")" && make "$@"
