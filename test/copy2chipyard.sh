#!/bin/bash

# Check if the user provided a file as input
if [ -z "$1" ]; then
    echo "Usage: $0 <file_to_transfer>"
    exit 1
fi

# Assign the input file to a variable
FILE_TO_TRANSFER="$1"

# Use scp to copy the file to the remote server
sudo cp "$FILE_TO_TRANSFER" /home/chipyard/chipyard/tests/

echo "COPY DONE"