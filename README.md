# Code-Juliett

* This project is just for testing my game framework project 'Code-Romeo'.
* To build it you need to compile the ShuildJuliett.c file with your compiler (clang/gcc/msvc). Then run it with arguments below. I will use clang for examples.

```shell
git clone --recurse-submodules https://github.com/omerfuyar/Code-Juliett.git
cd Code-Juliett
clang ShuildJuliett.c -o ShuildJuliett -O3
./ShuildJuliett clang r all
```

## Tests

This project includes a comprehensive test suite for the Romeo framework API. The tests cover:

- **Vector operations** - Vector2, Vector3, Vector4, and integer variants
- **String operations** - String creation, manipulation, and conversions
- **HashMap** - Hash map data structure functionality
- **ListArray** - Dynamic array operations
- **Timer** - Time measurement and tracking

### Building and Running Tests

To build and run all tests:

```shell
# Linux/Mac
./run_tests.sh clang d

# Windows
run_tests.bat clang d
```

For more details, see [tests/README.md](tests/README.md).