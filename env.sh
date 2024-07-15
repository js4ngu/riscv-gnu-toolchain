export RISCV=/opt/riscv

# Add RISCV/bin and RISCV/riscv64-unknown-elf/bin to PATH if not already included
if [[ ":$PATH:" != *":$RISCV/bin:"* ]]; then
    export PATH=$RISCV/bin:$PATH
fi

if [[ ":$PATH:" != *":$RISCV/riscv64-unknown-elf/bin:"* ]]; then
    export PATH=$RISCV/riscv64-unknown-elf/bin:$PATH
fi

# Set PK environment variable
export PK=$RISCV/riscv64-unknown-elf/bin/pk

# Debugging: Print the PATH and PK to verify they are set correctly
echo "PATH is set to: $PATH"
echo "PK is set to: $PK"
