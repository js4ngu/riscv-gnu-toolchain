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

# Change directory to the specified path
cd /home/jongsang/riscv-gnu-toolchain/test || exit 1

# Remove the old output file if it exists
rm -f "$OUTPUT_FILE"

# Compile the source file with gcc, enabling vector extension and linking the math library
#riscv64-unknown-elf-gcc -march=rv64gcv -mabi=lp64d -O2 "$SOURCE_FILE" -o "$OUTPUT_FILE" -lm
riscv64-unknown-elf-gcc -march=rv64gcv -mabi=lp64d -O2 -I. "$SOURCE_FILE" rope.c -o "$OUTPUT_FILE" -lm

# Print message after compilation is done
echo "Compile done for spike: $OUTPUT_FILE"