#ifndef EXAMPLES_BRAINFUCK_BUILD_H
#define EXAMPLES_BRAINFUCK_BUILD_H

#include <string_view>
#include <variant>

#include "examples/brainfuck/instructions.h"
#include "jasmin/core/function.h"

namespace bf {

struct parse_error {
  int line;
  int column;
};

// Returns a function from the program source text, or returns `parse_error` if
// the source text does not represent a valid Brainfuck program. Characters
// other than "[]+-<>,." are ignored, so the only way `contents` could not
// represent a valid program would be if the square brackets are mismatched.
std::variant<jasmin::Function<Instructions>, parse_error> BuildJasminFunction(
    std::string_view contents);

}  // namespace bf

#endif  // EXAMPLES_BRAINFUCK_BUILD_H
