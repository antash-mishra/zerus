# Zerus Game Engine

A cross-platform game engine written in pure C23 with modern best practices, memory safety, and comprehensive tooling.

## Features

- **Modern C23**: Uses the latest C standard with all modern language features
- **Memory Safety**: Comprehensive sanitizers and safety checks enabled
- **Cross-Platform**: CMake-based build system for maximum portability
- **VS Code Integration**: Full IDE support with debugging, formatting, and linting
- **Comprehensive Testing**: Unit tests with memory leak detection
- **Code Quality**: Automated formatting with clang-format and linting with clang-tidy

## Requirements

- CMake 3.20 or higher
- GCC or Clang compiler with C23 support
- clang-format (for code formatting)
- clang-tidy (for linting)
- GDB (for debugging)

## Quick Start

### Build

```bash
# Configure and build (Debug)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j4

# Run the engine
./build/zerus_engine
```

### Run Tests

```bash
# Run all tests
ctest --test-dir build --verbose

# Or run tests directly
./build/tests/test_engine
```

### Development with VS Code

1. Open the project in VS Code
2. Install recommended extensions:
   - C/C++ Extension Pack
   - CMake Tools
   - clangd
3. Use `Ctrl+Shift+P` → "CMake: Configure" to configure the project
4. Use `F5` to run and debug

## Project Structure

```
zerus/
├── src/                    # Source files
│   ├── main.c             # Main entry point
│   └── engine/            # Engine core systems
├── include/               # Header files
│   └── engine/           # Engine headers
├── tests/                # Unit tests
├── docs/                 # Documentation
├── assets/               # Game assets
├── examples/             # Example code
├── .vscode/              # VS Code configuration
├── CMakeLists.txt        # Build configuration
├── .clang-format         # Code formatting rules
├── .clang-tidy          # Linting rules
└── README.md            # This file
```

## Memory Safety

This project includes comprehensive memory safety features:

- **AddressSanitizer**: Detects buffer overflows, use-after-free, and memory leaks
- **UndefinedBehaviorSanitizer**: Catches undefined behavior at runtime  
- **Stack Protection**: Prevents stack buffer overflows
- **Control Flow Integrity**: Protects against ROP/JOP attacks
- **Position Independent Code**: Enhanced security through ASLR

## Code Style

- Code is automatically formatted with clang-format on save
- Snake_case naming convention for functions and variables
- Struct types end with `_t` suffix
- All compiler warnings are treated as errors
- Comprehensive linting with clang-tidy

## Build Configurations

### Debug (default)
- Optimizations disabled (`-O0`)
- Debug symbols included (`-g`)
- All sanitizers enabled
- Assertions enabled

### Release
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```
- Full optimizations (`-O3`)
- No debug symbols
- Assertions disabled
- Sanitizers disabled for performance

## Contributing

1. Code must pass all tests and linting checks
2. Follow the established code style
3. Add tests for new functionality
4. Update documentation as needed

## License

This project is licensed under the MIT License - see the LICENSE file for details.
