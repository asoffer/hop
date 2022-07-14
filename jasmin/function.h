#ifndef JASMIN_FUNCTION_H
#define JASMIN_FUNCTION_H

#include <type_traits>

#include "jasmin/call_stack.h"
#include "jasmin/instruction.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/internal/debug.h"
#include "jasmin/internal/function_base.h"

namespace jasmin {

// A representation of a function that ties op-codes to instructions (via an
// InstructionSet template parameter).
template <InstructionSet Set>
struct Function final : internal_function_base::FunctionBase {
  // Constructs an empty `Function` given a `parameter_count` representing the
  // number of parameters to the function, and a `return_count` representing the
  // number of return values for the function.
  explicit constexpr Function(uint8_t parameter_count, uint8_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  // Returns the number of instructions and values in the code for this
  // function.
  constexpr size_t size() const { return op_codes_.size(); }

  // Appends an op-code for the given `Instruction` template parameter, followed
  // by `Value`s for each of the passed arguments.
  template <typename Instruction, typename... Vs>
  constexpr void append(Vs... vs) requires((std::is_convertible_v<Vs, Value> and
                                            ...)) {
    op_codes_.push_back(
        OpCodeOrValue::OpCode(Set::template OpCodeFor<Instruction>()));
    (op_codes_.push_back(OpCodeOrValue::Value(vs)), ...);
  }

  // Appends an intsruction followed by space for `placeholder_count` values
  // which are left uninitialized. They may be initialized later via calls to
  // `OpCodeOrValue::set_value`. Returns the index of the first uninitialized
  // value.
  template <typename Instruction>
  constexpr size_t append_with_placeholders(size_t placeholders) {
    op_codes_.push_back(
        OpCodeOrValue::OpCode(Set::template OpCodeFor<Instruction>()));
    size_t result = op_codes_.size();
    op_codes_.resize(op_codes_.size() + placeholders,
                     OpCodeOrValue::UninitializedValue());
    return result;
  }

  // Given the index into the function where a `Value` is stored, overwrites the
  // `Value` with `value`. This is typically used in conjunction with
  // `append_with_placeholders` to defer setting a value, but may be used to
  // overwrite any `Value`. Behavior is undefined if `index` is larger than the
  // size of the function or if an op-code is stored at the given index.
  void set_value(size_t index, Value value) {
    JASMIN_INTERNAL_DEBUG_ASSERT(index < op_codes_.size(),
                                 "Index larger than function size");
    op_codes_[index].set_value(value);
  }
};

}  // namespace jasmin

#endif  // JASMIN_FUNCTION_H