#!/bin/bash

set -e

# Define directories
SRC_DIR="$(pwd)"
RESULT_DIR="$SRC_DIR/linux_result"
BUILD_DIR="$RESULT_DIR/build"
OUTPUT_DIR="$RESULT_DIR/output"
SRC_FILES="$SRC_DIR/EdgeClient.cpp"
HEADER_FILES="$SRC_DIR/EdgeClient.h"
LIB_NAME="libEdgeClient"
OBJ_FILE="$BUILD_DIR/EdgeClient.o"
STATIC_LIB="$OUTPUT_DIR/$LIB_NAME.a"
SHARED_LIB="$OUTPUT_DIR/$LIB_NAME.so"
SAMPLE_SRC="$SRC_DIR/utility/Sample.cpp"
SAMPLE_OBJ="$SRC_DIR/utility/Sample.o"
SAMPLE_EXE="$OUTPUT_DIR/Sample"

# Create result directories
mkdir -p "$RESULT_DIR" "$BUILD_DIR" "$OUTPUT_DIR"

# 1. Compile EdgeClient.cpp (for static & shared libraries)
g++ -std=c++20 -Wall -Wextra -O2 -fPIC -I"$SRC_DIR" -c "$SRC_FILES" -o "$OBJ_FILE"

# 2. Create static library
ar rcs "$STATIC_LIB" "$OBJ_FILE"

# 3. Create shared library
g++ -shared -o "$SHARED_LIB" "$OBJ_FILE"

# 4. Compile and link Sample executable
g++ -std=c++20 -Wall -Wextra -O2 -I"$SRC_DIR" -c "$SAMPLE_SRC" -o "$SAMPLE_OBJ"
g++ -std=c++20 -Wall -Wextra -O2 -o "$SAMPLE_EXE" "$SAMPLE_OBJ" "$STATIC_LIB"

# Move Sample.obj to utility directory
if [ -f "$SRC_DIR/utility/Sample.o" ]; then rm "$SRC_DIR/utility/Sample.o"; fi
if [ -f "$SAMPLE_OBJ" ]; then mv "$SAMPLE_OBJ" "$SRC_DIR/utility/Sample.o"; fi

echo "Build complete. Executable: $SAMPLE_EXE"
