@echo off
REM Test runner script for Romeo Framework tests
REM This script builds and runs all tests

setlocal enabledelayedexpansion

set COMPILER=%1
set BUILD_MODE=%2

if "%COMPILER%"=="" set COMPILER=clang
if "%BUILD_MODE%"=="" set BUILD_MODE=d

echo ========================================
echo Romeo Framework Test Suite
echo ========================================
echo Compiler: %COMPILER%
echo Build Mode: %BUILD_MODE% (d=debug, r=release)
echo.

REM Determine build directory
if "%BUILD_MODE%"=="d" (
    set BUILD_DIR=build\tests\debug
) else (
    set BUILD_DIR=build\tests\release
)

REM Build the test build system
echo Building test build system...
%COMPILER% ShuildTests.c -o ShuildTests.exe -O3
if errorlevel 1 goto error

REM Build all tests
echo Building all tests...
ShuildTests.exe %COMPILER% %BUILD_MODE% all
if errorlevel 1 goto error

echo.
echo ========================================
echo Running Tests
echo ========================================
echo.

REM Run all test executables
set TEST_FAILED=0

for %%f in (%BUILD_DIR%\test_*.exe) do (
    echo.
    echo Running %%~nf...
    echo ----------------------------------------
    "%%f"
    if errorlevel 1 set TEST_FAILED=1
)

echo.
echo ========================================
if %TEST_FAILED%==0 (
    echo ALL TEST SUITES PASSED
) else (
    echo SOME TESTS FAILED
)
echo ========================================

exit /b %TEST_FAILED%

:error
echo.
echo ========================================
echo BUILD FAILED
echo ========================================
exit /b 1
