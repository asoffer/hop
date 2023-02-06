#ifndef JASMIN_DEBUG_H
#define JASMIN_DEBUG_H

#include <string>
#include <string_view>

#include "jasmin/instruction.h"
#include "jasmin/value.h"

namespace jasmin {

// Returns a string representing the value stored in `v` in a human-readable
// hexadecimal format.
std::string ShowValue(Value v);

std::string ShowValueStack(ValueStack const& v);

template <Instruction I>
std::string_view InstructionName() {
  if constexpr (requires {
                  { I::name() } -> std::convertible_to<std::string_view>;
                }) {
    return I::name();
  } else {
    return typeid(I).name();
  }
}

struct DumpValueStack : StackMachineInstruction<DumpValueStack> {
  static std::string_view name() { return "dump-value-stack"; }

  static constexpr void execute(ValueStack& value_stack,
                                void (*fn)(std::string_view)) {
    fn(ShowValueStack(value_stack));
  }
};

}  // namespace jasmin

#endif  // JASMIN_DEBUG_H
