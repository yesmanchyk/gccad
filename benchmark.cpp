#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include "functions.h"
#include "differentiator.h"
#include "derivatives.h"

int main() {
  constexpr size_t N = 10'000'000;
  std::cout << "=== GCC Reflection AD Performance Benchmark ===" << std::endl;
  std::cout << "Iterations per test: " << N << "\n\n";

  // Prevent optimizer dead-code elimination
  volatile float sink = 0.0f;

  // ---------------------------------------------------------------------------
  // Benchmark 1: linear_poly (x * y + 3 * x)
  // ---------------------------------------------------------------------------
  std::cout << "[1] Benchmark: linear_poly (2 variables)" << std::endl;
  
  // A. Compiled Functor from derivatives.h
  {
    auto start = std::chrono::high_resolution_clock::now();
    float sum_x = 0.0f, sum_y = 0.0f;
    for (size_t i = 0; i < N; ++i) {
      func::linear_poly_functor fn;
      fn(2.0f + (i & 1) * 0.001f, 5.0f);
      fn.backward(1.0f);
      sum_x += fn.x_grad;
      sum_y += fn.y_grad;
    }
    auto end = std::chrono::high_resolution_clock::now();
    sink = sum_x + sum_y;
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "  - Functor (compiled C++ code) : " << std::fixed << std::setprecision(2) << elapsed_ms << " ms" << std::endl;
  }

  // B. Opcode Interpreter (op_code_exec)
  {
    constexpr auto lp_params = std::define_static_array(parameters_of( ^^func::linear_poly ));
    constexpr auto code_dx = generate_op_code_wrt( ^^func::linear_poly, lp_params[0]);
    constexpr auto code_dy = generate_op_code_wrt( ^^func::linear_poly, lp_params[1]);

    auto start = std::chrono::high_resolution_clock::now();
    float sum_x = 0.0f, sum_y = 0.0f;
    for (size_t i = 0; i < N; ++i) {
      float args[2] = { 2.0f + (i & 1) * 0.001f, 5.0f };
      sum_x += op_code_exec(code_dx, args);
      sum_y += op_code_exec(code_dy, args);
    }
    auto end = std::chrono::high_resolution_clock::now();
    sink = sum_x + sum_y;
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "  - op_code_exec (Interpreter)   : " << std::fixed << std::setprecision(2) << elapsed_ms << " ms" << std::endl;
  }

  std::cout << "\n";

  // ---------------------------------------------------------------------------
  // Benchmark 2: math_test (log(x) + sin(x) + cos(x) + pow(x, 3))
  // ---------------------------------------------------------------------------
  std::cout << "[2] Benchmark: math_test (transcendental functions)" << std::endl;

  // A. Compiled Functor from derivatives.h
  {
    auto start = std::chrono::high_resolution_clock::now();
    float sum = 0.0f;
    for (size_t i = 0; i < N; ++i) {
      func::math_test_functor fn;
      fn(2.0f + (i & 1) * 0.001f);
      fn.backward(1.0f);
      sum += fn.x_grad;
    }
    auto end = std::chrono::high_resolution_clock::now();
    sink = sum;
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "  - Functor (compiled C++ code) : " << std::fixed << std::setprecision(2) << elapsed_ms << " ms" << std::endl;
  }

  // B. Opcode Interpreter (op_code_exec)
  {
    constexpr auto math_params = std::define_static_array(parameters_of( ^^func::math_test ));
    constexpr auto code_dx = generate_op_code_wrt( ^^func::math_test, math_params[0]);

    auto start = std::chrono::high_resolution_clock::now();
    float sum = 0.0f;
    for (size_t i = 0; i < N; ++i) {
      float args[1] = { 2.0f + (i & 1) * 0.001f };
      sum += op_code_exec(code_dx, args);
    }
    auto end = std::chrono::high_resolution_clock::now();
    sink = sum;
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "  - op_code_exec (Interpreter)   : " << std::fixed << std::setprecision(2) << elapsed_ms << " ms" << std::endl;
  }

  std::cout << "\n";

  // ---------------------------------------------------------------------------
  // Benchmark 3: torch_test (3*a^3 - b^2)
  // ---------------------------------------------------------------------------
  std::cout << "[3] Benchmark: torch_test (multi-variable polynomial)" << std::endl;

  // A. Compiled Functor from derivatives.h
  {
    auto start = std::chrono::high_resolution_clock::now();
    float sum_a = 0.0f, sum_b = 0.0f;
    for (size_t i = 0; i < N; ++i) {
      func::torch_test_functor fn;
      fn(2.0f + (i & 1) * 0.001f, 6.0f);
      fn.backward(1.0f);
      sum_a += fn.a_grad;
      sum_b += fn.b_grad;
    }
    auto end = std::chrono::high_resolution_clock::now();
    sink = sum_a + sum_b;
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "  - Functor (compiled C++ code) : " << std::fixed << std::setprecision(2) << elapsed_ms << " ms" << std::endl;
  }

  // B. Opcode Interpreter (op_code_exec)
  {
    constexpr auto torch_params = std::define_static_array(parameters_of( ^^func::torch_test ));
    constexpr auto code_da = generate_op_code_wrt( ^^func::torch_test, torch_params[0]);
    constexpr auto code_db = generate_op_code_wrt( ^^func::torch_test, torch_params[1]);

    auto start = std::chrono::high_resolution_clock::now();
    float sum_a = 0.0f, sum_b = 0.0f;
    for (size_t i = 0; i < N; ++i) {
      float args[2] = { 2.0f + (i & 1) * 0.001f, 6.0f };
      sum_a += op_code_exec(code_da, args);
      sum_b += op_code_exec(code_db, args);
    }
    auto end = std::chrono::high_resolution_clock::now();
    sink = sum_a + sum_b;
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "  - op_code_exec (Interpreter)   : " << std::fixed << std::setprecision(2) << elapsed_ms << " ms" << std::endl;
  }

  (void)sink;
  return 0;
}
