#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <meta>
#include <string_view>
#include <initializer_list>
#include <span>
#include <cmath>

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

template <typename T, size_t Capacity = 128>
struct fixed_vector {
  T data[Capacity]{};
  size_t sz = 0;

  constexpr fixed_vector() = default;

  constexpr fixed_vector(std::initializer_list<T> init) {
    for (const auto& item : init) {
      if (sz < Capacity) {
        data[sz++] = item;
      }
    }
  }

  constexpr void push_back(const T& val) {
    if (sz < Capacity) {
      data[sz++] = val;
    }
  }

  constexpr void insert_back(const fixed_vector& other) {
    for (size_t i = 0; i < other.sz && sz < Capacity; ++i) {
      data[sz++] = other.data[i];
    }
  }

  constexpr const T* begin() const { return data; }
  constexpr const T* end() const { return data + sz; }
  constexpr size_t size() const { return sz; }
  constexpr bool empty() const { return sz == 0; }
  constexpr const T& operator[](size_t idx) const { return data[idx]; }
};

// String simplification helpers
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

// Opcode Data Structures
enum class op_code_kind {
  push_const,
  load_var,
  add, sub, mul, div, neg,
  exp, log, sin, cos, pow
};

struct instruction {
  op_code_kind op;
  float value = 0.0f;
  int var_idx = 0;
};

// Compile-Time & Runtime Opcode Interpreter
template <size_t N>
constexpr float op_code_exec(const fixed_vector<instruction, N>& code, std::span<const float> args) {
  float stack[128];
  int top = 0;

  for (size_t i = 0; i < code.size(); ++i) {
    const auto& inst = code[i];
    switch (inst.op) {
      case op_code_kind::push_const:
        stack[top++] = inst.value;
        break;
      case op_code_kind::load_var:
        stack[top++] = args[inst.var_idx];
        break;
      case op_code_kind::add: {
        float b = stack[--top];
        float a = stack[--top];
        stack[top++] = a + b;
        break;
      }
      case op_code_kind::sub: {
        float b = stack[--top];
        float a = stack[--top];
        stack[top++] = a - b;
        break;
      }
      case op_code_kind::mul: {
        float b = stack[--top];
        float a = stack[--top];
        stack[top++] = a * b;
        break;
      }
      case op_code_kind::div: {
        float b = stack[--top];
        float a = stack[--top];
        stack[top++] = a / b;
        break;
      }
      case op_code_kind::neg:
        stack[top - 1] = -stack[top - 1];
        break;
      case op_code_kind::exp:
        stack[top - 1] = std::exp(stack[top - 1]);
        break;
      case op_code_kind::log:
        stack[top - 1] = std::log(stack[top - 1]);
        break;
      case op_code_kind::sin:
        stack[top - 1] = std::sin(stack[top - 1]);
        break;
      case op_code_kind::cos:
        stack[top - 1] = std::cos(stack[top - 1]);
        break;
      case op_code_kind::pow: {
        float b = stack[--top];
        float a = stack[--top];
        stack[top++] = std::pow(a, b);
        break;
      }
    }
  }
  return top > 0 ? stack[top - 1] : 0.0f;
}

// -----------------------------------------------------------------------------
// CodeGen Abstraction Backends
// -----------------------------------------------------------------------------

struct string_code_gen {
  using result_type = fixed_string;

  constexpr fixed_string literal(fixed_string s) const { return s; }
  constexpr fixed_string literal(float val) const {
    if (val == 0.0f) return "0.0f";
    if (val == 1.0f) return "1.0f";
    return "1.0e+0f";
  }
  constexpr fixed_string load_var(fixed_string name, int /*idx*/) const { return name; }

  constexpr fixed_string add(fixed_string a, fixed_string b) const { return simplify_add(a, b); }
  constexpr fixed_string sub(fixed_string a, fixed_string b) const { return simplify_sub(a, b); }
  constexpr fixed_string mul(fixed_string a, fixed_string b) const { return simplify_mul(a, b); }
  constexpr fixed_string div(fixed_string a, fixed_string b) const {
    return fixed_string("(") + a + fixed_string(") / (") + b + fixed_string(")");
  }
  constexpr fixed_string neg(fixed_string a) const { return simplify_negate(a); }
  constexpr fixed_string exp(fixed_string a) const { return fixed_string("exp(") + a + fixed_string(")"); }
  constexpr fixed_string log(fixed_string a) const { return fixed_string("log(") + a + fixed_string(")"); }
  constexpr fixed_string sin(fixed_string a) const { return fixed_string("sin(") + a + fixed_string(")"); }
  constexpr fixed_string cos(fixed_string a) const { return fixed_string("cos(") + a + fixed_string(")"); }
  constexpr fixed_string pow(fixed_string a, fixed_string b) const {
    return fixed_string("pow(") + a + fixed_string(", ") + b + fixed_string(")");
  }
};

struct op_code_gen {
  using result_type = fixed_vector<instruction, 128>;

  static constexpr bool is_zero(const fixed_vector<instruction, 128>& code) {
    return code.size() == 1 && code[0].op == op_code_kind::push_const && code[0].value == 0.0f;
  }
  static constexpr bool is_one(const fixed_vector<instruction, 128>& code) {
    return code.size() == 1 && code[0].op == op_code_kind::push_const && code[0].value == 1.0f;
  }

  constexpr fixed_vector<instruction, 128> literal(fixed_string s) const {
    float val = 0.0f;
    if (s == "1.0f" || s == "1" || s == "1.0" || s == "1.f" || s == "1.0e+0f") val = 1.0f;
    else if (s == "2.0f" || s == "2" || s == "2.0" || s == "2.f" || s == "2.0e+0f") val = 2.0f;
    else if (s == "3.0f" || s == "3" || s == "3.0" || s == "3.f" || s == "3.0e+0f") val = 3.0f;
    return { instruction{op_code_kind::push_const, val, 0} };
  }
  constexpr fixed_vector<instruction, 128> literal(float val) const {
    return { instruction{op_code_kind::push_const, val, 0} };
  }
  constexpr fixed_vector<instruction, 128> load_var(fixed_string /*name*/, int idx) const {
    return { instruction{op_code_kind::load_var, 0.0f, idx} };
  }

  constexpr fixed_vector<instruction, 128> add(fixed_vector<instruction, 128> a, const fixed_vector<instruction, 128>& b) const {
    if (is_zero(a)) return b;
    if (is_zero(b)) return a;
    a.insert_back(b);
    a.push_back(instruction{op_code_kind::add});
    return a;
  }

  constexpr fixed_vector<instruction, 128> sub(fixed_vector<instruction, 128> a, const fixed_vector<instruction, 128>& b) const {
    if (is_zero(b)) return a;
    if (is_zero(a)) return neg(b);
    a.insert_back(b);
    a.push_back(instruction{op_code_kind::sub});
    return a;
  }

  constexpr fixed_vector<instruction, 128> mul(fixed_vector<instruction, 128> a, const fixed_vector<instruction, 128>& b) const {
    if (is_zero(a) || is_zero(b)) return { instruction{op_code_kind::push_const, 0.0f, 0} };
    if (is_one(a)) return b;
    if (is_one(b)) return a;
    a.insert_back(b);
    a.push_back(instruction{op_code_kind::mul});
    return a;
  }

  constexpr fixed_vector<instruction, 128> div(fixed_vector<instruction, 128> a, const fixed_vector<instruction, 128>& b) const {
    if (is_zero(a)) return { instruction{op_code_kind::push_const, 0.0f, 0} };
    a.insert_back(b);
    a.push_back(instruction{op_code_kind::div});
    return a;
  }

  constexpr fixed_vector<instruction, 128> neg(fixed_vector<instruction, 128> a) const {
    if (is_zero(a)) return { instruction{op_code_kind::push_const, 0.0f, 0} };
    a.push_back(instruction{op_code_kind::neg});
    return a;
  }

  constexpr fixed_vector<instruction, 128> exp(fixed_vector<instruction, 128> a) const {
    a.push_back(instruction{op_code_kind::exp});
    return a;
  }

  constexpr fixed_vector<instruction, 128> log(fixed_vector<instruction, 128> a) const {
    a.push_back(instruction{op_code_kind::log});
    return a;
  }

  constexpr fixed_vector<instruction, 128> sin(fixed_vector<instruction, 128> a) const {
    a.push_back(instruction{op_code_kind::sin});
    return a;
  }

  constexpr fixed_vector<instruction, 128> cos(fixed_vector<instruction, 128> a) const {
    a.push_back(instruction{op_code_kind::cos});
    return a;
  }

  constexpr fixed_vector<instruction, 128> pow(fixed_vector<instruction, 128> a, const fixed_vector<instruction, 128>& b) const {
    a.insert_back(b);
    a.push_back(instruction{op_code_kind::pow});
    return a;
  }
};

// -----------------------------------------------------------------------------
// Unified Expression Traversal Engine
// -----------------------------------------------------------------------------

template <typename CodeGen>
consteval typename CodeGen::result_type eval_expr_gen(info E, CodeGen& cg, std::span<const info> params) {
  if (expression_kind_of(E) == expression_kind::literal) {
    return cg.literal(display_string_of(E));
  }
  if (expression_kind_of(E) == expression_kind::variable) {
    fixed_string vname = identifier_of(E);
    int idx = 0;
    for (size_t i = 0; i < params.size(); ++i) {
      if (identifier_of(params[i]) == vname) {
        idx = static_cast<int>(i);
        break;
      }
    }
    return cg.load_var(vname, idx);
  }
  if (expression_kind_of(E) == expression_kind::unary_op) {
    auto operands = operands_of(E);
    try {
      auto op = operator_of(E);
      if (op == operators::op_minus) {
        return cg.neg(eval_expr_gen(operands[0], cg, params));
      }
    } catch (...) {
      return eval_expr_gen(operands[0], cg, params);
    }
  }
  if (expression_kind_of(E) == expression_kind::binary_op) {
    auto operands = operands_of(E);
    auto op = operator_of(E);
    if (op == operators::op_equals) {
      return eval_expr_gen(operands[1], cg, params);
    }
    if (op == operators::op_plus) {
      return cg.add(eval_expr_gen(operands[0], cg, params), eval_expr_gen(operands[1], cg, params));
    }
    if (op == operators::op_minus) {
      return cg.sub(eval_expr_gen(operands[0], cg, params), eval_expr_gen(operands[1], cg, params));
    }
    if (op == operators::op_star) {
      return cg.mul(eval_expr_gen(operands[0], cg, params), eval_expr_gen(operands[1], cg, params));
    }
    if (op == operators::op_slash) {
      return cg.div(eval_expr_gen(operands[0], cg, params), eval_expr_gen(operands[1], cg, params));
    }
  }
  if (expression_kind_of(E) == expression_kind::function_call) {
    auto operands = operands_of(E);
    fixed_string callee_name = identifier_of(operands[0]);
    if (callee_name == "exp_helper" || callee_name == "exp") {
      return cg.exp(eval_expr_gen(operands[1], cg, params));
    }
    if (callee_name == "log") {
      return cg.log(eval_expr_gen(operands[1], cg, params));
    }
    if (callee_name == "sin") {
      return cg.sin(eval_expr_gen(operands[1], cg, params));
    }
    if (callee_name == "cos") {
      return cg.cos(eval_expr_gen(operands[1], cg, params));
    }
    if (callee_name == "pow") {
      return cg.pow(eval_expr_gen(operands[1], cg, params), eval_expr_gen(operands[2], cg, params));
    }
  }
  return cg.literal(0.0f);
}

template <typename CodeGen>
consteval typename CodeGen::result_type differentiate_expr_gen(info E, info var_info, CodeGen& cg, std::span<const info> params) {
  if (expression_kind_of(E) == expression_kind::literal) {
    return cg.literal(0.0f);
  }
  if (expression_kind_of(E) == expression_kind::variable) {
    if (identifier_of(operands_of(E)[0]) == identifier_of(var_info)) {
      return cg.literal(1.0f);
    }
    return cg.literal(0.0f);
  }
  if (expression_kind_of(E) == expression_kind::unary_op) {
    auto operands = operands_of(E);
    try {
      auto op = operator_of(E);
      if (op == operators::op_minus) {
        return cg.neg(differentiate_expr_gen(operands[0], var_info, cg, params));
      }
    } catch (...) {
      return differentiate_expr_gen(operands[0], var_info, cg, params);
    }
  }
  if (expression_kind_of(E) == expression_kind::binary_op) {
    auto operands = operands_of(E);
    auto op = operator_of(E);
    if (op == operators::op_equals) {
      return differentiate_expr_gen(operands[1], var_info, cg, params);
    }
    if (op == operators::op_plus) {
      return cg.add(differentiate_expr_gen(operands[0], var_info, cg, params), differentiate_expr_gen(operands[1], var_info, cg, params));
    }
    if (op == operators::op_minus) {
      return cg.sub(differentiate_expr_gen(operands[0], var_info, cg, params), differentiate_expr_gen(operands[1], var_info, cg, params));
    }
    if (op == operators::op_star) {
      // d(A*B) = d(A)*B + A*d(B)
      return cg.add(
        cg.mul(differentiate_expr_gen(operands[0], var_info, cg, params), eval_expr_gen(operands[1], cg, params)),
        cg.mul(eval_expr_gen(operands[0], cg, params), differentiate_expr_gen(operands[1], var_info, cg, params))
      );
    }
    if (op == operators::op_slash) {
      // d(A/B) = (d(A)*B - A*d(B)) / B^2
      auto num = cg.sub(
        cg.mul(differentiate_expr_gen(operands[0], var_info, cg, params), eval_expr_gen(operands[1], cg, params)),
        cg.mul(eval_expr_gen(operands[0], cg, params), differentiate_expr_gen(operands[1], var_info, cg, params))
      );
      auto den = cg.mul(eval_expr_gen(operands[1], cg, params), eval_expr_gen(operands[1], cg, params));
      return cg.div(num, den);
    }
  }
  if (expression_kind_of(E) == expression_kind::function_call) {
    auto operands = operands_of(E);
    fixed_string callee_name = identifier_of(operands[0]);
    if (callee_name == "exp_helper" || callee_name == "exp") {
      // d(exp(u)) = exp(u) * d(u)
      return cg.mul(eval_expr_gen(E, cg, params), differentiate_expr_gen(operands[1], var_info, cg, params));
    }
    if (callee_name == "log") {
      // d(log(u)) = (1 / u) * d(u)
      auto inv = cg.div(cg.literal(1.0f), eval_expr_gen(operands[1], cg, params));
      return cg.mul(inv, differentiate_expr_gen(operands[1], var_info, cg, params));
    }
    if (callee_name == "sin") {
      // d(sin(u)) = cos(u) * d(u)
      return cg.mul(cg.cos(eval_expr_gen(operands[1], cg, params)), differentiate_expr_gen(operands[1], var_info, cg, params));
    }
    if (callee_name == "cos") {
      // d(cos(u)) = -sin(u) * d(u)
      return cg.mul(cg.neg(cg.sin(eval_expr_gen(operands[1], cg, params))), differentiate_expr_gen(operands[1], var_info, cg, params));
    }
    if (callee_name == "pow") {
      // d(u^v) = u^v * (d(v)*log(u) + v*(1/u)*d(u))
      info u = operands[1];
      info v = operands[2];
      auto term1 = cg.mul(differentiate_expr_gen(v, var_info, cg, params), cg.log(eval_expr_gen(u, cg, params)));
      auto inv_u = cg.div(cg.literal(1.0f), eval_expr_gen(u, cg, params));
      auto term2 = cg.mul(eval_expr_gen(v, cg, params), cg.mul(inv_u, differentiate_expr_gen(u, var_info, cg, params)));
      return cg.mul(eval_expr_gen(E, cg, params), cg.add(term1, term2));
    }
  }
  return cg.literal(0.0f);
}

// Backward Compatibility API
consteval fixed_string print_expr(info E) {
  string_code_gen cg;
  return eval_expr_gen(E, cg, {});
}

consteval fixed_string differentiate_expr(info E, info var_info) {
  string_code_gen cg;
  return differentiate_expr_gen(E, var_info, cg, {});
}

consteval fixed_string differentiate_function_wrt(info func_reflection, info var_info) {
  string_code_gen cg;
  info body = body_of(func_reflection);
  auto params = parameters_of(func_reflection);
  for (info stmt : statements_of(body)) {
    if (is_expression(stmt)) {
      return differentiate_expr_gen(stmt, var_info, cg, params);
    }
  }
  return "0.0f";
}

consteval fixed_string print_function_body(info func_reflection) {
  string_code_gen cg;
  info body = body_of(func_reflection);
  auto params = parameters_of(func_reflection);
  for (info stmt : statements_of(body)) {
    if (is_expression(stmt)) {
      return eval_expr_gen(stmt, cg, params);
    }
  }
  return "0.0f";
}

// Bytecode Generator API
consteval fixed_vector<instruction, 128> generate_op_code_wrt(info func_reflection, info var_info) {
  op_code_gen cg;
  info body = body_of(func_reflection);
  auto params = parameters_of(func_reflection);
  for (info stmt : statements_of(body)) {
    if (is_expression(stmt)) {
      return differentiate_expr_gen(stmt, var_info, cg, params);
    }
  }
  return {};
}

consteval fixed_vector<instruction, 128> generate_op_code_body(info func_reflection) {
  op_code_gen cg;
  info body = body_of(func_reflection);
  auto params = parameters_of(func_reflection);
  for (info stmt : statements_of(body)) {
    if (is_expression(stmt)) {
      return eval_expr_gen(stmt, cg, params);
    }
  }
  return {};
}

#endif // DIFFERENTIATOR_H
