#!/bin/bash
# Compile and run Golden Model CPU

clear

# Default hex file
HEX_FILE="logisim-bin/sum.hex"

# Check if hex file argument is provided
if [ $# -gt 0 ]; then
    HEX_FILE="$1"
fi

# Create build directory if it doesn't exist
BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

# Compile
echo "Compiling golden_model_cpu.cpp..."
g++ -o "$BUILD_DIR/golden_model_cpu" golden_model_cpu.cpp -std=c++11 -Wall

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Error: Compilation failed!"
    exit 1
fi

# Run with hex file
echo "Running golden_model_cpu with $HEX_FILE..."
./"$BUILD_DIR/golden_model_cpu" "$HEX_FILE"

