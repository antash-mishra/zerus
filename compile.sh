#!/bin/bash

# Path to the GLSL compiler
GLSLC_PATH="/home/antash/Android/Sdk/ndk/27.0.12077973/shader-tools/linux-x86_64/glslc"

# --- Validation ---
# Check if the compiler exists
if [ ! -f "$GLSLC_PATH" ]; then
    echo "Error: glslc not found at $GLSLC_PATH" >&2
    exit 1
fi

# Check for correct number of arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input_shader.glsl> <output_shader.spv>" >&2
    exit 1
fi

INPUT_FILE=$1
OUTPUT_FILE=$2

# --- Compilation ---
$GLSLC_PATH "$INPUT_FILE" -o "$OUTPUT_FILE"

# Check for compilation success
if [ $? -ne 0 ]; then
    echo "Error: Shader compilation failed for '$INPUT_FILE'" >&2
    exit 1
fi

echo "Compilation complete." 