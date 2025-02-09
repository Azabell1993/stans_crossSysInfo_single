#!/bin/bash

set -e

# Define directories
SRC_DIR=src
INCLUDE_DIR=include
BUILD_DIR=build
OUTPUT_DIR=output
SRC_FILES="$SRC_DIR/EdgeClient.cpp"
HEADER_FILES="$INCLUDE_DIR/EdgeClient.h"
LIB_NAME="libEdgeClient"

# Clean build directories
rm -rf $BUILD_DIR $OUTPUT_DIR
mkdir -p $BUILD_DIR $OUTPUT_DIR

# Detect platform
OS=$(uname -s)

# Compiler and linker settings
if [[ "$OS" == "Darwin" ]]; then
    # macOS build
    CXX=g++
    CXXFLAGS="-std=c++20 -Wall -Wextra -O2 -I$INCLUDE_DIR"
    STATIC_LIB=$OUTPUT_DIR/$LIB_NAME.a
    SHARED_LIB=$OUTPUT_DIR/$LIB_NAME.dylib
elif [[ "$OS" == "Linux" ]]; then
    # Linux build
    CXX=g++
    CXXFLAGS="-std=c++20 -Wall -Wextra -O2 -I$INCLUDE_DIR"
    STATIC_LIB=$OUTPUT_DIR/$LIB_NAME.a
    SHARED_LIB=$OUTPUT_DIR/$LIB_NAME.so
elif [[ "$OS" == "MINGW64_NT"* ]] || [[ "$OS" == "CYGWIN_NT"* ]]; then
    # Windows build using MSVC
    CXX=cl
    CXXFLAGS="/std:c++20 /W4 /O2 /I$INCLUDE_DIR"
    STATIC_LIB=$OUTPUT_DIR/$LIB_NAME.lib
    SHARED_LIB=$OUTPUT_DIR/$LIB_NAME.dll
    OBJ_FILE=$BUILD_DIR/EdgeClient.obj
    
    # Compile
    echo "Compiling for Windows (MSVC)..."
    $CXX $CXXFLAGS /c $SRC_FILES /Fo$OBJ_FILE
    
    # Create static library
    lib /OUT:$STATIC_LIB $OBJ_FILE
    
    # Create shared library
    cl /LD $OBJ_FILE /Fe$SHARED_LIB
else
    echo "Unsupported OS: $OS"
    exit 1
fi

# Compile for Unix-based systems
if [[ "$OS" == "Darwin" || "$OS" == "Linux" ]]; then
    echo "Compiling for $OS..."
    $CXX $CXXFLAGS -c $SRC_FILES -o $BUILD_DIR/EdgeClient.o
    
    # Create static library
    ar rcs $STATIC_LIB $BUILD_DIR/EdgeClient.o
    
    # Create shared library
    if [[ "$OS" == "Darwin" ]]; then
        $CXX -dynamiclib -o $SHARED_LIB $BUILD_DIR/EdgeClient.o
    elif [[ "$OS" == "Linux" ]]; then
        $CXX -shared -o $SHARED_LIB $BUILD_DIR/EdgeClient.o
    fi
fi

echo "Build complete. Files are in $OUTPUT_DIR:"