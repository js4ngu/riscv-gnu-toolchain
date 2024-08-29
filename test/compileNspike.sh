#!/bin/bash

# Clear the terminal
clear

# Check if a source file is provided as an argument
if [ -z "$1" ]; then
  echo "Usage: $0 <source_file>"
  exit 1
fi

# Set the file name to compile from the first argument
SOURCE_FILE="$1"

# Derive the output file name by appending _gdb before the extension
OUTPUT_FILE="${SOURCE_FILE%.*}.riscv"

# Compile
./compile.sh "$SOURCE_FILE"

# Spike run
./runSpike.sh "$OUTPUT_FILE"