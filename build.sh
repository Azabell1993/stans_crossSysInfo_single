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

# 결과 디렉터리 생성
mkdir -p "$RESULT_DIR" "$BUILD_DIR" "$OUTPUT_DIR"

# 1. EdgeClient.cpp 컴파일 (정적 및 공유 라이브러리를 위한 객체 파일 생성)
g++ -std=c++20 -Wall -Wextra -O2 -fPIC -D_GLIBCXX_USE_CXX11_ABI=1 $INCLUDE_FLAGS -c "$SRC_FILES" -o "$OBJ_FILE"

# 2. 정적 라이브러리 생성
ar rcs "$STATIC_LIB" "$OBJ_FILE"

# 3. 공유 라이브러리 생성
g++ -shared -o "$SHARED_LIB" "$OBJ_FILE"

# 4. 샘플 실행 파일 컴파일 및 링크
g++ -std=c++20 -Wall -Wextra -O2 -D_GLIBCXX_USE_CXX11_ABI=1 $INCLUDE_FLAGS -c "$SAMPLE_SRC" -o "$SAMPLE_OBJ"
g++ -std=c++20 -Wall -Wextra -O2 -o "$SAMPLE_EXE" "$SAMPLE_OBJ" "$STATIC_LIB" $LIB_FLAGS

echo "Build complete. Executable: $SAMPLE_EXE"