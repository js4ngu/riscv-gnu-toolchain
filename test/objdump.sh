#!/bin/bash

# Check if a source file is provided as an argument
if [ -z "$1" ]; then
  echo "Usage: $0 <source_file>"
  exit 1
fi

# Set the file name to compile from the first argument
SOURCE_FILE="$1"

# Derive the output file name by replacing the extension with .objdump
OUTPUT_FILE="${SOURCE_FILE%.*}.objdump"

# Use objdump to disassemble the source file and write the output to the new file
riscv64-unknown-elf-objdump -d "$SOURCE_FILE" > "$OUTPUT_FILE"

# Print a success message
echo "Disassembly complete. Output saved to $OUTPUT_FILE"
