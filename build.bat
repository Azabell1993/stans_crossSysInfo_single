@echo off
setlocal

:: Visual Studio Developer Command Prompt 환경 설정
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

set SRC_DIR=C:\Users\jeewo\Desktop\crossSysInfo\utils
set RESULT_DIR=%SRC_DIR%\window_result
set BUILD_DIR=%RESULT_DIR%\build
set OUTPUT_DIR=%RESULT_DIR%\output
set SRC_FILES=%SRC_DIR%\EdgeClient.cpp
set HEADER_FILES=%SRC_DIR%\EdgeClient.h
set LIB_NAME=EdgeClient
set OBJ_FILE=%BUILD_DIR%\EdgeClient.obj
set STATIC_LIB=%OUTPUT_DIR%\%LIB_NAME%.lib
set SHARED_LIB=%OUTPUT_DIR%\%LIB_NAME%.dll
set SAMPLE_SRC=%SRC_DIR%\utility\Sample.cpp
set SAMPLE_EXE=%OUTPUT_DIR%\Sample.exe

:: fmt 및 spdlog 포함 설정 (vcpkg 사용)
set VCPKG_DIR=C:\vcpkg
set INCLUDE_FLAGS=/utf-8 /I%VCPKG_DIR%\installed\x64-windows\include /I%SRC_DIR%
set LIB_FLAGS=/LIBPATH:%VCPKG_DIR%\installed\x64-windows\lib fmt.lib spdlog.lib

:: Create result directories
if not exist %RESULT_DIR% mkdir %RESULT_DIR%
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

:: 1. EdgeClient.cpp 컴파일 (정적 & 동적 라이브러리용)
cl /std:c++20 /EHsc /W4 /O2 %INCLUDE_FLAGS% /c %SRC_FILES% /Fo%OBJ_FILE%

:: 2. 정적 라이브러리 생성
lib /OUT:%STATIC_LIB% %OBJ_FILE%

:: 3. 동적 라이브러리 생성
cl /LD %OBJ_FILE% /Fe%SHARED_LIB%

:: 4. 실행 파일 Sample.exe 빌드
cl /std:c++20 /EHsc /W4 /O2 %INCLUDE_FLAGS% %SAMPLE_SRC% /link /OUT:%SAMPLE_EXE% %STATIC_LIB% %LIB_FLAGS%

echo Build complete. Executable: %SAMPLE_EXE%
