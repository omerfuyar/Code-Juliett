#!/bin/bash

# Test runner script for Romeo Framework tests
# This script builds and runs all tests

set -e

COMPILER=${1:-clang}
BUILD_MODE=${2:-d}

echo "========================================"
echo "Romeo Framework Test Suite"
echo "========================================"
echo "Compiler: $COMPILER"
echo "Build Mode: $BUILD_MODE (d=debug, r=release)"
echo ""

# Determine build directory
if [ "$BUILD_MODE" = "d" ]; then
    BUILD_DIR="build/tests/debug"
else
    BUILD_DIR="build/tests/release"
fi

# Build the test build system
echo "Building test build system..."
$COMPILER ShuildTests.c -o ShuildTests -O3

# Build all tests
echo "Building all tests..."
./ShuildTests $COMPILER $BUILD_MODE all

echo ""
echo "========================================"
echo "Running Tests"
echo "========================================"
echo ""

# Run all test executables
TEST_FAILED=0

for test in $BUILD_DIR/test_*; do
    if [ -x "$test" ]; then
        echo ""
        echo "Running $(basename $test)..."
        echo "----------------------------------------"
        if ! "$test"; then
            TEST_FAILED=1
        fi
    fi
done

echo ""
echo "========================================"
if [ $TEST_FAILED -eq 0 ]; then
    echo "ALL TEST SUITES PASSED ✓"
else
    echo "SOME TESTS FAILED ✗"
fi
echo "========================================"

exit $TEST_FAILED
