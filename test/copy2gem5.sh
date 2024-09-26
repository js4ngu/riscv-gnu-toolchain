#!/bin/bash
clear

if [ -z "$1" ]; then
  echo "Usage: $0 <source_file>"
  exit 1
fi

SOURCE_FILE="$1"

OUTPUT_FILE="${SOURCE_FILE%.*}.riscv"

docker cp ./"$SOURCE_FILE" gem5-container:/home/gem5/tests/riscv-elf/

echo "Copy Done"