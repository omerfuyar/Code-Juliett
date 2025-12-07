# Romeo Framework Tests

This directory contains comprehensive tests for the Romeo game framework API.

## Test Structure

The tests are organized into separate files, each focusing on a specific module:

- **test_vector.c** - Tests for Vector2, Vector3, Vector4, and their integer variants
- **test_string.c** - Tests for String and StringView operations
- **test_hashmap.c** - Tests for HashMap data structure
- **test_listarray.c** - Tests for ListArray (dynamic array) data structure
- **test_timer.c** - Tests for Timer and TimePoint functionality

## Building Tests

To build the tests, first compile the test build system:

```bash
clang ShuildTests.c -o ShuildTests -O3
```

Then run it to build all tests:

```bash
./ShuildTests clang d all    # Build in debug mode with all dependencies
./ShuildTests clang r all    # Build in release mode with all dependencies
```

The compiled test executables will be placed in `build/tests/debug/` or `build/tests/release/`.

## Running Tests

After building, you can run individual test suites:

```bash
./build/tests/debug/test_vector
./build/tests/debug/test_string
./build/tests/debug/test_hashmap
./build/tests/debug/test_listarray
./build/tests/debug/test_timer
```

Or run all tests:

```bash
./build/tests/debug/test_vector && \
./build/tests/debug/test_string && \
./build/tests/debug/test_hashmap && \
./build/tests/debug/test_listarray && \
./build/tests/debug/test_timer
```

## Test Framework

The tests use a simple, custom test framework defined in `test_framework.h` with the following macros:

- `TEST_ASSERT(condition, message)` - Assert a condition is true
- `TEST_ASSERT_EQUAL(actual, expected, message)` - Assert two values are equal
- `TEST_ASSERT_FLOAT_EQUAL(actual, expected, epsilon, message)` - Assert two floats are approximately equal
- `RUN_TEST(test_func)` - Run a test function

Each test file follows the same pattern:
1. Include the test framework
2. Include the module being tested
3. Define test functions
4. Run tests in main()
5. Print summary and return result

## What is Tested

### Vector Tests
- Vector creation and initialization
- Arithmetic operations (add, scale)
- Geometric operations (magnitude, normalize, dot product, cross product)
- Interpolation (lerp)
- Directional vectors (up, down, left, right, forward, backward)
- Integer vector variants
- Color operations

### String Tests
- String creation and destruction
- String views and literals
- Comparison operations
- Concatenation (beginning and end)
- String modification
- Type conversions (to float, to int)
- Tokenization
- Macro helpers (scc, scl, scv)

### HashMap Tests
- Creation and destruction
- Registering and accessing values
- Updating existing values
- Handling nonexistent keys
- Multiple data types
- Struct storage
- Capacity expansion
- Many entries handling

### ListArray Tests
- Creation and destruction
- Adding and getting elements
- Setting elements
- Adding ranges
- Removing elements (by index, range, item)
- Popping elements
- Clearing the list
- Finding indices
- Copying lists
- Resizing capacity
- Inserting at specific indices

### Timer Tests
- Timer creation and destruction
- Starting and stopping timers
- Measuring elapsed time
- Time unit conversions (milliseconds, nanoseconds)
- Timer reset functionality
- TimePoint operations
- Multiple start/stop cycles
- Timing precision

## Notes

- Tests are designed to be clear and focused on specific functionality
- Each test function tests a single aspect of the API
- Tests avoid performance benchmarking and focus on correctness
- The test structure is kept clean and organized
