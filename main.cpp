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

  // Using static constexpr to give it a constant address for template for loop
  static constexpr auto members = std::define_static_array(members_of( ^^func, access_context::unchecked() ));
  
  template for (constexpr auto member : members) {
    if (is_function(member)) {
      constexpr auto name = identifier_of(member);
      constexpr auto deriv_expr = differentiate_function(member);
      
      out << "constexpr float " << name.data() << "_derivative(float x) {\n";
      out << "  // Symbolic derivative generated at compile time via C++26 reflection\n";
      out << "  return " << deriv_expr.data << ";\n";
      out << "}\n\n";
    }
  }

  out << "} // namespace func\n\n";
  out << "#endif // DERIVATIVES_H\n";
  std::cout << "Successfully generated derivatives.h!\n";
  return 0;
}
