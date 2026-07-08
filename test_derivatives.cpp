#include <iostream>
#include <cmath>
#include <cassert>
#include "functions.h"
#include "derivatives.h"

int main() {
  float x = 1.0f;
  
  // Test exp_helper
  float calculated_exp = func::exp_helper_derivative(x);
  float expected_exp = std::exp(x);
  std::cout << "exp_helper_derivative(" << x << ") = " << calculated_exp << " (expected: " << expected_exp << ")\n";
  assert(std::abs(calculated_exp - expected_exp) < 1e-5f);

  // Test sigmoid
  float calculated_sig = func::sigmoid_derivative(x);
  float expected_sig = -std::exp(-x);
  std::cout << "sigmoid_derivative(" << x << ") = " << calculated_sig << " (expected: " << expected_sig << ")\n";
  assert(std::abs(calculated_sig - expected_sig) < 1e-5f);

  // Test custom_poly
  float calculated_poly = func::custom_poly_derivative(x);
  float expected_poly = 2.0f * x + 2.0f;
  std::cout << "custom_poly_derivative(" << x << ") = " << calculated_poly << " (expected: " << expected_poly << ")\n";
  assert(std::abs(calculated_poly - expected_poly) < 1e-5f);

  std::cout << "All verification tests PASSED successfully!\n";
  return 0;
}
