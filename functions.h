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

} // namespace func

#endif // FUNCTIONS_H
