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

spike --isa=rv64gcv pk "$SOURCE_FILE"