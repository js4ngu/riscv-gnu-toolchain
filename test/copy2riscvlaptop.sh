#!/bin/bash

# Check if the user provided a file as input
if [ -z "$1" ]; then
    echo "Usage: $0 <file_to_transfer>"
    exit 1
fi

# Assign the input file to a variable
FILE_TO_TRANSFER="$1"

# Define the remote server details
REMOTE_USER="esca"
REMOTE_SERVER="163.152.30.120"
REMOTE_PORT="2685"
REMOTE_PATH="/home/esca/jongsang"

# Use scp to copy the file to the remote server
scp -P "$REMOTE_PORT" "$FILE_TO_TRANSFER" "$REMOTE_USER@$REMOTE_SERVER:$REMOTE_PATH"

# Check if the transfer was successful
if [ $? -eq 0 ]; then
    echo "File '$FILE_TO_TRANSFER' successfully transferred to $REMOTE_USER@$REMOTE_SERVER:$REMOTE_PATH"
else
    echo "File transfer failed."
fi
