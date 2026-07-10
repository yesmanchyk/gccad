#include <iostream>
#include <cmath>
#include <cassert>
#include "functions.h"
#include "derivatives.h"

int main() {
  // 1. Verify exp_helper_functor
  func::exp_helper_functor exp_fn;
  float val_exp = exp_fn(1.5f);
  exp_fn.backward(2.0f); // out_grad = 2.0f
  float expected_exp_grad = std::exp(1.5f) * 2.0f;
  std::cout << "exp_helper_functor gradient: " << exp_fn.x_grad << " (expected: " << expected_exp_grad << ")\n";
  assert(std::abs(exp_fn.x_grad - expected_exp_grad) < 1e-5f);

  // 2. Verify sigmoid_functor
  func::sigmoid_functor sig_fn;
  float val_sig = sig_fn(1.0f);
  sig_fn.backward(1.5f); // out_grad = 1.5f
  float expected_sig_grad = -std::exp(-1.0f) * 1.5f;
  std::cout << "sigmoid_functor gradient: " << sig_fn.x_grad << " (expected: " << expected_sig_grad << ")\n";
  assert(std::abs(sig_fn.x_grad - expected_sig_grad) < 1e-5f);

  // 3. Verify custom_poly_functor
  func::custom_poly_functor poly_fn;
  float val_poly = poly_fn(2.0f);
  poly_fn.backward(0.5f); // out_grad = 0.5f
  float expected_poly_grad = (2.0f * 2.0f + 2.0f) * 0.5f; // (2x + 2) * out_grad = 6 * 0.5 = 3.0f
  std::cout << "custom_poly_functor gradient: " << poly_fn.x_grad << " (expected: " << expected_poly_grad << ")\n";
  assert(std::abs(poly_fn.x_grad - expected_poly_grad) < 1e-5f);

  // 4. Verify linear_poly_functor (multi-parameter adjoint)
  func::linear_poly_functor lp_fn;
  float val_lp = lp_fn(2.0f, 5.0f); // x = 2.0, y = 5.0
  lp_fn.backward(3.0f); // out_grad = 3.0f
  
  // df/dx = y + 3 = 8.0f. grad_x = 8.0 * 3.0 = 24.0f
  float expected_lp_x_grad = (5.0f + 3.0f) * 3.0f;
  // df/dy = x = 2.0f. grad_y = 2.0 * 3.0 = 6.0f
  float expected_lp_y_grad = 2.0f * 3.0f;

  std::cout << "linear_poly_functor x_grad: " << lp_fn.x_grad << " (expected: " << expected_lp_x_grad << ")\n";
  std::cout << "linear_poly_functor y_grad: " << lp_fn.y_grad << " (expected: " << expected_lp_y_grad << ")\n";
  assert(std::abs(lp_fn.x_grad - expected_lp_x_grad) < 1e-5f);
  assert(std::abs(lp_fn.y_grad - expected_lp_y_grad) < 1e-5f);

  // 5. Verify math_test_functor (log, sin, cos, pow)
  func::math_test_functor math_fn;
  float val_math = math_fn(2.0f); // x = 2.0f
  math_fn.backward(1.5f); // out_grad = 1.5f
  
  float expected_math_grad = (1.0f / 2.0f + std::cos(2.0f) - std::sin(2.0f) + 3.0f * 2.0f * 2.0f) * 1.5f;
  std::cout << "math_test_functor gradient: " << math_fn.x_grad << " (expected: " << expected_math_grad << ")\n";
  assert(std::abs(math_fn.x_grad - expected_math_grad) < 1e-4f);

  // 6. Verify [2, 6] vector of torch_test_functor
  func::torch_test_functor torch_fn;
  float a = 2.0f, b = 6.0f;
  float val_torch = torch_fn(a, b);
  torch_fn.backward(1.0f);
  
  float expected_torch_a_grad = 9*a*a;
  std::cout << "torch_test_functor a: " << torch_fn.a << ", a_grad: " << torch_fn.a_grad << " (expected: " << expected_torch_a_grad << ")\n";
  assert(std::abs(torch_fn.a_grad - expected_torch_a_grad) < 1e-4f);

  float expected_torch_b_grad = -2*b;
  std::cout << "torch_test_functor b: " << torch_fn.b << ", b_grad: " << torch_fn.b_grad << " (expected: " << expected_torch_b_grad << ")\n";
  assert(std::abs(torch_fn.b_grad - expected_torch_b_grad) < 1e-4f);


  std::cout << "All functor verification tests PASSED successfully!\n";
  return 0;
}
