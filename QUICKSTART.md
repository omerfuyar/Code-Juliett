# Quick Start Guide - Romeo Framework Tests

This is a quick reference for building and running the tests.

## Prerequisites

1. Clone the repository with submodules:
```bash
git clone --recurse-submodules https://github.com/omerfuyar/Code-Juliett.git
cd Code-Juliett
```

2. Build the Romeo framework dependency (requires fixing upstream bugs in Romeo's shuild.h first):
```bash
# Once Romeo's build system is fixed:
cd dependencies/Romeo
clang ShuildRomeo.c -o ShuildRomeo -O3
./ShuildRomeo clang d all
cd ../..
```

## Running Tests

### Option 1: Use the Test Runner (Recommended)

**Linux/Mac:**
```bash
chmod +x run_tests.sh
./run_tests.sh clang d
```

**Windows:**
```bat
run_tests.bat clang d
```

### Option 2: Build and Run Manually

**Step 1: Build the test build system**
```bash
clang ShuildTests.c -o ShuildTests -O3
```

**Step 2: Build all tests**
```bash
./ShuildTests clang d all
```

**Step 3: Run individual tests**
```bash
./build/tests/debug/test_vector
./build/tests/debug/test_string
./build/tests/debug/test_hashmap
./build/tests/debug/test_listarray
./build/tests/debug/test_timer
```

## Compiler Options

The scripts support different compilers:
- `clang` - LLVM Clang
- `gcc` - GNU Compiler Collection
- `cl` or `clang-cl` - Microsoft Visual C++ (Windows)

## Build Modes

- `d` - Debug mode (with debug symbols and assertions)
- `r` - Release mode (optimized, no debug info)

## Test Output

Each test will output:
- `[PASS]` for successful tests
- `[FAIL]` for failed tests with line numbers
- Summary at the end with total/passed/failed counts

Example:
```
Running test_vector2_creation...
  [PASS] test_vector2_creation: Vector2 x component should be 3.0
  [PASS] test_vector2_creation: Vector2 y component should be 4.0
  
========================================
Test Suite: Vector Tests
========================================
Total:  27
Passed: 27
Failed: 0
========================================
RESULT: ALL TESTS PASSED
```

## What Gets Tested

- **Vector Math** (27 tests) - 2D, 3D, 4D vectors and operations
- **String Operations** (13 tests) - String creation, manipulation, conversion
- **HashMap** (8 tests) - Hash table data structure operations
- **ListArray** (13 tests) - Dynamic array operations
- **Timer** (10 tests) - Time measurement and tracking

**Total: 71 tests**

## Documentation

For more details, see:
- [tests/README.md](tests/README.md) - Complete test documentation
- [tests/TEST_COVERAGE.md](tests/TEST_COVERAGE.md) - Detailed coverage report
- [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) - Implementation details

## Troubleshooting

**Issue:** "Romeo framework not built"
- **Solution:** Build Romeo framework first (see Prerequisites section)

**Issue:** Compilation errors in Romeo's shuild.h
- **Solution:** This is a known issue in the Romeo framework that needs to be fixed upstream

**Issue:** Permission denied on Linux/Mac
- **Solution:** Make scripts executable: `chmod +x run_tests.sh`

**Issue:** Test failures on slow systems
- **Solution:** Timer tests have generous tolerances but may occasionally fail on very slow systems - this is expected

## Need Help?

Check the detailed documentation in the [tests/](tests/) directory or the implementation summary at the root of the repository.
