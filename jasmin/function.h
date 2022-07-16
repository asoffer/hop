#ifndef JASMIN_FUNCTION_H
#define JASMIN_FUNCTION_H

#include <type_traits>

#include "jasmin/call_stack.h"
#include "jasmin/instruction.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/internal/debug.h"
#include "jasmin/internal/function_base.h"

namespace jasmin {
namespace internal_function {

template <
    internal_instruction::SignatureSatisfiesRequirementsWithImmediateValues E,
    typename... Vs>
constexpr bool ConvertibleArguments = E::invoke_with_argument_types([
]<std::same_as<ValueStack&>, typename... Ts>() {
  return (std::convertible_to<Vs, Ts> and ...);
});

}  // namespace internal_function

// A representation of a function that ties op-codes to instructions (via an
// InstructionSet template parameter).
template <InstructionSet Set>
struct Function final : internal_function_base::FunctionBase {
  // Constructs an empty `Function` given a `parameter_count` representing the
  // number of parameters to the function, and a `return_count` representing the
  // number of return values for the function.
  explicit constexpr Function(uint8_t parameter_count, uint8_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  // Appends an op-code for the given `Instruction I` template parameter,
  // followed by `Value`s for each of the passed arguments.
  template <Instruction I, typename... Vs>
  constexpr void append(Vs... vs) requires(
      internal_function::ConvertibleArguments<
          internal::ExtractSignature<decltype(&I::execute)>, Vs...>) {
    op_codes_.push_back(OpCodeOrValue::OpCode(Set::template OpCodeFor<I>()));
    internal::ExtractSignature<decltype(&I::execute)>::
        invoke_with_argument_types([&]<std::same_as<ValueStack&>,
                                       typename... Ts>() {
          return (std::convertible_to<Vs, Ts> and ...);
        });

    (op_codes_.push_back(OpCodeOrValue::Value(vs)), ...);
  }
  // Same as `append` above for instructions with no immediate values.
  template <Instruction I>
  constexpr void append() requires(
      internal_instruction::ImmediateValueCount<I>() == 0) {
    op_codes_.push_back(OpCodeOrValue::OpCode(Set::template OpCodeFor<I>()));
  }

  // Appends an intsruction followed by space for `placeholder_count` values
  // which are left uninitialized. They may be initialized later via calls to
  // `OpCodeOrValue::set_value`. Returns the size of the function after all
  // placeholders are saved.
  template <Instruction I>
  constexpr size_t append_with_placeholders() {
    constexpr size_t placeholders =
        internal_instruction::ImmediateValueCount<I>();
    op_codes_.push_back(OpCodeOrValue::OpCode(Set::template OpCodeFor<I>()));
    op_codes_.resize(op_codes_.size() + placeholders,
                     OpCodeOrValue::UninitializedValue());
   return op_codes_.size();
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
