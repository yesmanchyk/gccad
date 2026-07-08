#include <iostream>
#include <fstream>
#include "functions.h"
#include "differentiator.h"

int main() {
  std::ofstream out("derivatives.h");
  if (!out) {
    std::cerr << "Failed to open derivatives.h\n";
    return 1;
  }
  out << "#ifndef DERIVATIVES_H\n";
  out << "#define DERIVATIVES_H\n\n";
  out << "#include <cmath>\n\n";
  out << "namespace func {\n\n";

  // Loop over all members of the func namespace
  static constexpr auto members = std::define_static_array(members_of( ^^func, access_context::unchecked() ));
  
  template for (constexpr auto member : members) {
    if (is_function(member)) {
      constexpr auto name = identifier_of(member);
      static constexpr auto params = std::define_static_array(parameters_of(member));
      constexpr auto orig_expr = print_function_body(member);

      out << "struct " << name.data() << "_functor {\n";
      
      // 1. Generate member variables and gradients for all parameters
      template for (constexpr auto p : params) {
        constexpr auto p_name = identifier_of(p);
        out << "  float " << p_name.data() << ";\n";
        out << "  float " << p_name.data() << "_grad = 0.0f;\n";
      }
      out << "\n";
      
      // 2. Generate operator() storing inputs and evaluating function
      out << "  constexpr float operator()(";
      bool first = true;
      template for (constexpr auto p : params) {
        if (!first) out << ", ";
        first = false;
        out << "float " << identifier_of(p).data();
      }
      out << ") {\n";
      template for (constexpr auto p : params) {
        constexpr auto p_name = identifier_of(p);
        out << "    this->" << p_name.data() << " = " << p_name.data() << ";\n";
      }
      out << "    return " << orig_expr.data << ";\n";
      out << "  }\n\n";
      
      // 3. Generate backward calculating partial derivatives for each parameter
      out << "  constexpr void backward(float out_grad = 1.0f) {\n";
      template for (constexpr auto p : params) {
        constexpr auto p_name = identifier_of(p);
        constexpr auto partial_deriv = differentiate_function_wrt(member, p);
        out << "    this->" << p_name.data() << "_grad += (" << partial_deriv.data << ") * out_grad;\n";
      }
      out << "  }\n";
      out << "};\n\n";
    }
  }

  out << "} // namespace func\n\n";
  out << "#endif // DERIVATIVES_H\n";
  std::cout << "Successfully generated derivatives.h!\n";
  return 0;
}
