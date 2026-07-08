#ifndef DERIVATIVES_H
#define DERIVATIVES_H

#include <cmath>

namespace func {

constexpr float exp_helper_derivative(float x) {
  // Symbolic derivative generated at compile time via C++26 reflection
  return exp(x);
}

constexpr float sigmoid_derivative(float x) {
  // Symbolic derivative generated at compile time via C++26 reflection
  return (exp_helper(-(x))) * (-(1.0f));
}

constexpr float custom_poly_derivative(float x) {
  // Symbolic derivative generated at compile time via C++26 reflection
  return ((x) + (x)) + (2.0e+0f);
}

} // namespace func

#endif // DERIVATIVES_H
