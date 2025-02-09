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
SAMPLE_OBJ="$BUILD_DIR/Sample.o"
SAMPLE_EXE="$OUTPUT_DIR/Sample"

# fmt 및 spdlog 포함 설정
INCLUDE_FLAGS="-I$SRC_DIR"
LIB_FLAGS="-L/usr/lib/x86_64-linux-gnu -lfmt -lspdlog -pthread"

# Create result directories
mkdir -p "$RESULT_DIR" "$BUILD_DIR" "$OUTPUT_DIR"

# 1. Compile EdgeClient.cpp (for static & shared libraries)
g++ -std=c++20 -Wall -Wextra -O2 -fPIC -D_GLIBCXX_USE_CXX11_ABI=1 $INCLUDE_FLAGS -c "$SRC_FILES" -o "$OBJ_FILE"

# 2. Create static library
ar rcs "$STATIC_LIB" "$OBJ_FILE"

# 3. Create shared library
g++ -shared -o "$SHARED_LIB" "$OBJ_FILE"

# 4. Compile and link Sample executable
g++ -std=c++20 -Wall -Wextra -O2 -D_GLIBCXX_USE_CXX11_ABI=1 $INCLUDE_FLAGS -c "$SAMPLE_SRC" -o "$SAMPLE_OBJ"
g++ -std=c++20 -Wall -Wextra -O2 -o "$SAMPLE_EXE" "$SAMPLE_OBJ" "$STATIC_LIB" $LIB_FLAGS

echo "Build complete. Executable: $SAMPLE_EXE"
