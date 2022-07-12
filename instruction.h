#ifndef JASMIN_INSTRUCTIONX_H
#define JASMIN_INSTRUCTIONX_H

#include <cassert>
#include <type_traits>
#include <utility>

#include "jasmin/call_stack.h"
#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"

namespace jasmin {

template <typename... Instructions>
struct InstructionTable {
  using function_type = Function<InstructionTable>;

  template <typename Instruction>
  static constexpr uint64_t OpCodeFor() requires(
      (std::is_same_v<Instruction, Instructions> or ...)) {
    constexpr size_t value = OpCodeForImpl<Instruction>();
    return value;
  }

  static constexpr void (*table[sizeof...(Instructions)])(ValueStack &,
                                                          InstructionPointer &,
                                                          CallStack &) = {
      &Instructions::template execute_impl<InstructionTable>...};

 private:
  template <typename Instruction>
  static constexpr uint64_t OpCodeForImpl() requires(
      (std::is_same_v<Instruction, Instructions> or ...)) {
    uint64_t i = 0;
    static_cast<void>(
        ((std::is_same_v<Instruction, Instructions> ? true : (++i, false)) or
         ...));
    return i;
  }
};

template <typename InstructionTableType>
void Execute(Function<InstructionTableType> const &f, ValueStack &value_stack) {
  CallStack call_stack;
  InstructionPointer ip = f.entry();
  call_stack.push(&f, value_stack.size(), ip);
  return InstructionTableType::table[ip->op_code()](value_stack, ip,
                                                    call_stack);
}

template <typename InstructionTableType>
void Execute(Function<InstructionTableType> const &f,
             std::initializer_list<Value> arguments,
             SmallTrivialValue auto &...return_values) {
  ValueStack value_stack(arguments);
  int dummy;
  Execute(f, value_stack);
  (dummy = ... = (return_values =
                      value_stack.pop<std::decay_t<decltype(return_values)>>(),
                  0));
}

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONX_H
