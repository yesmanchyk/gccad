#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cmath>

namespace func {

constexpr float exp_helper(float x) {
  return std::exp(x);
}

constexpr float sigmoid(float x) {
  return 1.0f + (exp_helper(-x) + 1.0f);
}

constexpr float custom_poly(float x) {
  return x * x + 2.0f * x;
}

constexpr float linear_poly(float x, float y) {
  return x * y + 3.0f * x;
}

constexpr float math_test(float x) {
  return std::log(x) + std::sin(x) + std::cos(x) + std::pow(x, 3.0f);
}

// Q function from https://docs.pytorch.org/tutorials/beginner/blitz/autograd_tutorial.html#differentiation-in-autograd
float torch_test(float a, float b) {
  return 3*std::pow(a, 3.0f) - std::pow(b, 2.0f);
}

} // namespace func

#endif // FUNCTIONS_H
