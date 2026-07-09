#ifndef DERIVATIVES_H
#define DERIVATIVES_H

#include <cmath>

namespace func {

struct exp_helper_functor {
  float x;
  float x_grad = 0.0f;

  constexpr float operator()(float x) {
    this->x = x;
    return exp(x);
  }

  constexpr void backward(float out_grad = 1.0f) {
    this->x_grad += (exp(x)) * out_grad;
  }
};

struct sigmoid_functor {
  float x;
  float x_grad = 0.0f;

  constexpr float operator()(float x) {
    this->x = x;
    return ((exp_helper(-(x)) + 1.0e+0f) + 1.0e+0f);
  }

  constexpr void backward(float out_grad = 1.0f) {
    this->x_grad += ((exp_helper(-(x))) * (-(1.0f))) * out_grad;
  }
};

struct custom_poly_functor {
  float x;
  float x_grad = 0.0f;

  constexpr float operator()(float x) {
    this->x = x;
    return ((x * x) + (x * 2.0e+0f));
  }

  constexpr void backward(float out_grad = 1.0f) {
    this->x_grad += (((x) + (x)) + (2.0e+0f)) * out_grad;
  }
};

struct linear_poly_functor {
  float x;
  float x_grad = 0.0f;
  float y;
  float y_grad = 0.0f;

  constexpr float operator()(float x, float y) {
    this->x = x;
    this->y = y;
    return ((x * y) + (x * 3.0e+0f));
  }

  constexpr void backward(float out_grad = 1.0f) {
    this->x_grad += ((y) + (3.0e+0f)) * out_grad;
    this->y_grad += (x) * out_grad;
  }
};

struct math_test_functor {
  float x;
  float x_grad = 0.0f;

  constexpr float operator()(float x) {
    this->x = x;
    return (((log(x) + sin(x)) + cos(x)) + pow(x, 3.0e+0f));
  }

  constexpr void backward(float out_grad = 1.0f) {
    this->x_grad += ((((1.0e+0f / (x)) + (cos(x))) + (-sin(x))) + ((pow(x, 3.0e+0f)) * ((3.0e+0f) * (1.0e+0f / (x))))) * out_grad;
  }
};

} // namespace func

#endif // DERIVATIVES_H
