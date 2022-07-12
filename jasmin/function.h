#ifndef JASMIN_FUNCTION_H
#define JASMIN_FUNCTION_H

#include <cassert>
#include <type_traits>

#include "jasmin/call_stack.h"
#include "jasmin/internal/function_base.h"

namespace jasmin {

// TODO: Remove the need for this forward-declaration.
struct JumpIf;

template <typename InstructionTable>
struct Function : internal_function_base::FunctionBase {
  explicit constexpr Function(uint8_t parameter_count, uint8_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  template <typename Instruction, typename... Vs>
  constexpr void append(Vs... vs) requires((std::is_convertible_v<Vs, Value> and
                                            ...)) {
    op_codes_.push_back(OpCodeOrValue::OpCode(
        InstructionTable::template OpCodeFor<Instruction>()));
    (op_codes_.push_back(OpCodeOrValue::Value(vs)), ...);
  }

  size_t append_conditional_jump() {
    append<JumpIf>(ptrdiff_t{0});
    size_t result = op_codes_.size() - 1;
    return result;
  }

  void set_jump_target(size_t index) {
    assert(op_codes_.size() > index);
    op_codes_[index].set_value(static_cast<ptrdiff_t>(op_codes_.size() - 1));
  }
};

}  // namespace jasmin

#endif  // JASMIN_FUNCTION_H
