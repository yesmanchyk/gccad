#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <meta>
#include <string_view>

using namespace std::meta;

struct fixed_string {
  char data[512]{};
  size_t len = 0;

  constexpr fixed_string() = default;
  constexpr fixed_string(const char* s) {
    while (s[len] && len < 511) {
      data[len] = s[len];
      ++len;
    }
    data[len] = '\0';
  }
  constexpr fixed_string(std::string_view sv) {
    len = sv.size() < 511 ? sv.size() : 511;
    for (size_t i = 0; i < len; ++i) {
      data[i] = sv[i];
    }
    data[len] = '\0';
  }

  constexpr fixed_string operator+(const fixed_string& other) const {
    fixed_string res;
    for (size_t i = 0; i < len; ++i) {
      res.data[i] = data[i];
    }
    res.len = len;
    for (size_t i = 0; i < other.len && res.len < 511; ++i) {
      res.data[res.len++] = other.data[i];
    }
    res.data[res.len] = '\0';
    return res;
  }

  constexpr bool operator==(std::string_view sv) const {
    std::string_view self(data, len);
    return self == sv;
  }
};

// Simple simplification helpers to avoid cluttering derivatives with 0 and 1
constexpr fixed_string simplify_add(fixed_string a, fixed_string b) {
  if (a == "0.0f" || a == "0" || a == "0.0" || a == "0.f") return b;
  if (b == "0.0f" || b == "0" || b == "0.0" || b == "0.f") return a;
  return fixed_string("(") + a + fixed_string(") + (") + b + fixed_string(")");
}

constexpr fixed_string simplify_sub(fixed_string a, fixed_string b) {
  if (b == "0.0f" || b == "0" || b == "0.0" || b == "0.f") return a;
  if (a == "0.0f" || a == "0" || a == "0.0" || a == "0.f") return fixed_string("-(") + b + fixed_string(")");
  return fixed_string("(") + a + fixed_string(") - (") + b + fixed_string(")");
}

constexpr fixed_string simplify_mul(fixed_string a, fixed_string b) {
  if (a == "0.0f" || a == "0" || a == "0.0" || a == "0.f" || b == "0.0f" || b == "0" || b == "0.0" || b == "0.f") {
    return "0.0f";
  }
  if (a == "1.0f" || a == "1" || a == "1.0" || a == "1.f") return b;
  if (b == "1.0f" || b == "1" || b == "1.0" || b == "1.f") return a;
  return fixed_string("(") + a + fixed_string(") * (") + b + fixed_string(")");
}

constexpr fixed_string simplify_negate(fixed_string a) {
  if (a == "0.0f" || a == "0" || a == "0.0" || a == "0.f") return "0.0f";
  return fixed_string("-(") + a + fixed_string(")");
}

// Forward declarations
consteval fixed_string print_expr(info E);
consteval fixed_string differentiate_expr(info E, info var_info);

consteval fixed_string print_expr(info E) {
  if (expression_kind_of(E) == expression_kind::literal) {
    return display_string_of(E);
  }
  if (expression_kind_of(E) == expression_kind::variable) {
    return identifier_of(E);
  }
  if (expression_kind_of(E) == expression_kind::unary_op) {
    auto operands = operands_of(E);
    try {
      auto op = operator_of(E);
      if (op == operators::op_minus) {
        return fixed_string("-(") + print_expr(operands[0]) + fixed_string(")");
      }
    } catch (...) {
      // It is a conversion/cast wrapper; skip it
      return print_expr(operands[0]);
    }
  }
  if (expression_kind_of(E) == expression_kind::binary_op) {
    auto operands = operands_of(E);
    auto op = operator_of(E);
    if (op == operators::op_equals) {
      return print_expr(operands[1]); // Value of assignment is the RHS
    }
    fixed_string op_str = " + ";
    if (op == operators::op_minus) op_str = " - ";
    if (op == operators::op_star) op_str = " * ";
    if (op == operators::op_slash) op_str = " / ";

    return fixed_string("(") + print_expr(operands[0]) + op_str + print_expr(operands[1]) + fixed_string(")");
  }
  if (expression_kind_of(E) == expression_kind::function_call) {
    auto operands = operands_of(E);
    fixed_string callee_name = identifier_of(operands[0]);
    fixed_string args_str = "";
    for (size_t i = 1; i < operands.size(); ++i) {
      if (i > 1) args_str = args_str + fixed_string(", ");
      args_str = args_str + print_expr(operands[i]);
    }
    return callee_name + fixed_string("(") + args_str + fixed_string(")");
  }
  return "<unknown>";
}

consteval fixed_string differentiate_expr(info E, info var_info) {
  if (expression_kind_of(E) == expression_kind::literal) {
    return "0.0f";
  }
  if (expression_kind_of(E) == expression_kind::variable) {
    // Check if it is the variable we are differentiating with respect to
    if (identifier_of(operands_of(E)[0]) == identifier_of(var_info)) {
      return "1.0f";
    }
    return "0.0f";
  }
  if (expression_kind_of(E) == expression_kind::unary_op) {
    auto operands = operands_of(E);
    try {
      auto op = operator_of(E);
      if (op == operators::op_minus) {
        return simplify_negate(differentiate_expr(operands[0], var_info));
      }
    } catch (...) {
      // Propagation through cast/conversion node
      return differentiate_expr(operands[0], var_info);
    }
  }
  if (expression_kind_of(E) == expression_kind::binary_op) {
    auto operands = operands_of(E);
    auto op = operator_of(E);
    if (op == operators::op_equals) {
      return differentiate_expr(operands[1], var_info);
    }
    if (op == operators::op_plus) {
      return simplify_add(differentiate_expr(operands[0], var_info), differentiate_expr(operands[1], var_info));
    }
    if (op == operators::op_minus) {
      return simplify_sub(differentiate_expr(operands[0], var_info), differentiate_expr(operands[1], var_info));
    }
    if (op == operators::op_star) {
      // d(A*B) = d(A)*B + A*d(B)
      return simplify_add(
        simplify_mul(differentiate_expr(operands[0], var_info), print_expr(operands[1])),
        simplify_mul(print_expr(operands[0]), differentiate_expr(operands[1], var_info))
      );
    }
    if (op == operators::op_slash) {
      // d(A/B) = (d(A)*B - A*d(B)) / B^2
      fixed_string num = simplify_sub(
        simplify_mul(differentiate_expr(operands[0], var_info), print_expr(operands[1])),
        simplify_mul(print_expr(operands[0]), differentiate_expr(operands[1], var_info))
      );
      fixed_string den = simplify_mul(print_expr(operands[1]), print_expr(operands[1]));
      return fixed_string("(") + num + fixed_string(") / (") + den + fixed_string(")");
    }
  }
  if (expression_kind_of(E) == expression_kind::function_call) {
    auto operands = operands_of(E);
    fixed_string callee_name = identifier_of(operands[0]);
    if (callee_name == "exp_helper" || callee_name == "exp") {
      // d(exp(u)) = exp(u) * d(u)
      return simplify_mul(print_expr(E), differentiate_expr(operands[1], var_info));
    }
    if (callee_name == "log") {
      // d(log(u)) = (1 / u) * d(u)
      fixed_string inv = fixed_string("1.0e+0f / (") + print_expr(operands[1]) + fixed_string(")");
      return simplify_mul(inv, differentiate_expr(operands[1], var_info));
    }
    if (callee_name == "sin") {
      // d(sin(u)) = cos(u) * d(u)
      fixed_string cos_val = fixed_string("cos(") + print_expr(operands[1]) + fixed_string(")");
      return simplify_mul(cos_val, differentiate_expr(operands[1], var_info));
    }
    if (callee_name == "cos") {
      // d(cos(u)) = -sin(u) * d(u)
      fixed_string neg_sin_val = fixed_string("-sin(") + print_expr(operands[1]) + fixed_string(")");
      return simplify_mul(neg_sin_val, differentiate_expr(operands[1], var_info));
    }
    if (callee_name == "pow") {
      // d(u^v) = u^v * (d(v)*log(u) + v*(1/u)*d(u))
      info u = operands[1];
      info v = operands[2];
      fixed_string term1 = simplify_mul(differentiate_expr(v, var_info), fixed_string("log(") + print_expr(u) + fixed_string(")"));
      fixed_string inv_u = fixed_string("1.0e+0f / (") + print_expr(u) + fixed_string(")");
      fixed_string term2 = simplify_mul(print_expr(v), simplify_mul(inv_u, differentiate_expr(u, var_info)));
      return simplify_mul(print_expr(E), simplify_add(term1, term2));
    }
  }
  return "0.0f";
}

consteval fixed_string differentiate_function(info func_reflection) {
  info body = body_of(func_reflection);
  auto params = parameters_of(func_reflection);
  if (params.size() == 0) return "0.0f";
  info var_info = params[0]; // Differentiate with respect to the first parameter

  // Find the return statement/expression
  for (info stmt : statements_of(body)) {
    // If it's a return statement or contains return/assignment, differentiate it
    if (is_expression(stmt)) {
      return differentiate_expr(stmt, var_info);
    }
  }
  return "0.0f";
}

consteval fixed_string differentiate_function_wrt(info func_reflection, info var_info) {
  info body = body_of(func_reflection);
  for (info stmt : statements_of(body)) {
    if (is_expression(stmt)) {
      return differentiate_expr(stmt, var_info);
    }
  }
  return "0.0f";
}

consteval fixed_string print_function_body(info func_reflection) {
  info body = body_of(func_reflection);
  for (info stmt : statements_of(body)) {
    if (is_expression(stmt)) {
      return print_expr(stmt);
    }
  }
  return "0.0f";
}

#endif // DIFFERENTIATOR_H
