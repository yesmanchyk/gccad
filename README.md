# GCC Reflection Automatic Differentiation Library (`gccad`)

A C++26 reflection-based header-only symbolic automatic differentiation library and code generator built on GCC's reflection implementation (`-freflection`).

## Project Structure

- **[functions.h](functions.h)**: Target C++ functions defined in `namespace func` (e.g. `sigmoid`, `linear_poly`, `math_test`, `torch_test`).
- **[differentiator.h](differentiator.h)**: Header-only C++26 symbolic differentiator that traverses AST statement/expression nodes at compile time and calculates partial derivatives.
- **[main.cpp](main.cpp)**: Generator executable driver that reflects all functions in `namespace func` using `members_of`, iterates over them using `template for`, and generates PyTorch-like functor structs into `derivatives.h`.
- **[derivatives.h](derivatives.h)**: Generated header containing `constexpr` functor structs (`<func>_functor`) with member variables, gradient storage, and `.backward(float out_grad)` methods.
- **[test_derivatives.cpp](test_derivatives.cpp)**: Mathematical verification test suite.
- **[benchmark.cpp](benchmark.cpp)**: Performance benchmark suite comparing compiled functor execution against `op_code_exec` bytecode interpreter speed.
- **[CMakeLists.txt](CMakeLists.txt)**: CMake build configuration.
- **[Makefile](Makefile)**: GNU Make build configuration.

---

## Environment & GCC Setup

To build and run the reflection generator, you need the custom GCC reflection compiler branch:

```bash
git clone --branch body_of --depth 1 \
  https://github.com/yesmanchyk/gcc-mirror
docker build -t gccb .
docker run --rm -it -v `pwd`:/src gccb
cd gcc-mirror # or /src/gcc-mirror
contrib/download_prerequisites
cd ..
mkdir build
cd build
../gcc-mirror/configure --enable-languages=c,c++ \
  --disable-bootstrap --disable-multilib --disable-nls \
  --prefix=/src/local
make -j8
make install
cd ..
local/bin/g++ -std=c++26 -freflection main.cpp -o generator
./generator
```

The generator produces `derivatives.h` which can be included into an application like `test_derivatives.cpp`.

---

## Build & Usage Instructions

### Prerequisites

All builds require the C++26 reflection-enabled GCC compiler (`-freflection`).

Custom compiler path inside Docker container:
`/src/gcc/yesmanchyk/gcc-mirror-install/bin/g++`

---

### Building with CMake

1. **Configure CMake build directory**:
   ```bash
   docker exec -w /src/gcc/yesmanchyk/ad gccad cmake -B build -DCMAKE_CXX_COMPILER=/src/gcc/yesmanchyk/gcc-mirror-install/bin/g++
   ```

2. **Build targets (compiles `generator`, executes it to output `derivatives.h`, and compiles `test_derivatives` and `benchmark`)**:
   ```bash
   docker exec -w /src/gcc/yesmanchyk/ad gccad cmake --build build
   ```

3. **Run verification test suite**:
   ```bash
   docker exec -w /src/gcc/yesmanchyk/ad/build gccad ./test_derivatives
   ```

4. **Run performance benchmark**:
   ```bash
   docker exec -w /src/gcc/yesmanchyk/ad/build gccad ./benchmark
   ```

---

### Building with GNU Make

Alternatively, build and run directly using Make:

```bash
# Build generator, execute it to produce derivatives.h, and compile test_derivatives
docker exec -w /src/gcc/yesmanchyk/ad gccad make

# Build and execute verification tests
docker exec -w /src/gcc/yesmanchyk/ad gccad make run

# Build and execute performance benchmark
docker exec -w /src/gcc/yesmanchyk/ad gccad make bench

# Clean build artifacts
docker exec -w /src/gcc/yesmanchyk/ad gccad make clean
```

---

## Performance Benchmark Results

Comparing execution speed of compiled PyTorch-like Functors (`derivatives.h`) vs. Opcode Interpreter (`op_code_exec`) over **10,000,000 iterations** compiled with `-O3`:

```
=== GCC Reflection AD Performance Benchmark ===
Iterations per test: 10,000,000

[1] Benchmark: linear_poly (2 variables)
  - Functor (compiled C++ code) : 6.02 ms
  - op_code_exec (Interpreter)   : 618.99 ms   (~102x speed difference)

[2] Benchmark: math_test (transcendental functions: log, sin, cos, pow)
  - Functor (compiled C++ code) : 125.74 ms
  - op_code_exec (Interpreter)   : 205.85 ms   (~1.6x speed difference)

[3] Benchmark: torch_test (multi-variable polynomial)
  - Functor (compiled C++ code) : 80.77 ms
  - op_code_exec (Interpreter)   : 1384.55 ms  (~17x speed difference)
```

### Key Takeaways

- **Compiled Functors (`derivatives.h`)**: Offer native, fully vectorized C++ execution speed. Ideal for high-throughput runtime computation.
- **Opcode Interpreter (`op_code_exec`)**: Executes opcode vectors directly in `consteval`/`constexpr` context. Ideal for zero-code-size compile-time constant evaluation (`static_assert`) or dynamic runtime evaluation without emitting new binary symbols.

