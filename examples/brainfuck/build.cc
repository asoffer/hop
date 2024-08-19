#include "examples/brainfuck/build.h"
#include "examples/brainfuck/instructions.h"

namespace bf {

std::variant<hop::Function<Instructions>, parse_error> BuildHopFunction(
    std::string_view contents) {
  hop::Function<Instructions> f(0, 0);
  f.append<Initialize>();
  std::vector<nth::interval<hop::InstructionIndex>> open_brackets;
  int line   = 1;
  int column = 0;
  for (char c : contents) {
    switch (c) {
      case '+': f.append<Increment>(); break;
      case '-': f.append<Decrement>(); break;
      case '<': f.append<Left>(); break;
      case '>': f.append<Right>(); break;
      case '[': {
        f.append<Zero>();
        open_brackets.push_back(f.append_with_placeholders<hop::JumpIf>());
      } break;
      case ']': {
        if (open_brackets.empty()) {
          return parse_error{.line = line, .column = column};
        }
        f.append<Zero>();
        auto loc = open_brackets.back();
        open_brackets.pop_back();
        auto land = f.append_with_placeholders<hop::JumpIfNot>();
        f.set_value(loc, 0, land.lower_bound() - loc.lower_bound() + 2);
        f.set_value(land, 0, loc.lower_bound() - land.lower_bound() + 2);
      } break;
      case ',': f.append<Input>(); break;
      case '.': f.append<Output>(); break;
      case '\n':
        ++line;
        column = 0;
        break;
      default: break;
    }
    ++column;
  }

  if (not open_brackets.empty()) {
    return parse_error{.line = line, .column = column};
  }

  f.append<hop::Return>();
  return f;
}

}  // namespace bf
