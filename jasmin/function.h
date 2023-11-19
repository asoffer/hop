#ifndef JASMIN_FUNCTION_H
#define JASMIN_FUNCTION_H

#include "jasmin/call_stack.h"
#include "jasmin/instruction.h"
#include "jasmin/instruction_index.h"
#include "jasmin/internal/function_base.h"
#include "jasmin/internal/instruction_traits.h"
#include "nth/container/interval.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {
namespace internal {

template <typename I, typename Set>
concept ContainedIn = Set::instructions.template contains<nth::type<I>>();

}  // namespace internal

// A representation of a function that ties op-codes to instructions (via an
// InstructionSet template parameter).
template <InstructionSet Set>
struct Function : internal::FunctionBase {
 private:
  template <Instruction I>
  static Value ExecPtr() {
    return &I::template ExecuteImpl<typename Set::self_type>;
  }

 public:
  // Constructs an empty `Function` given a `parameter_count` representing
  // the number of parameters to the function, and a `return_count`
  // representing the number of return values for the function.
  explicit constexpr Function(uint8_t parameter_count, uint8_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  // Appends an op-code for the given `Instruction I` template parameter.
  template <internal::ContainedIn<Set> I>
  constexpr nth::interval<InstructionIndex> append(auto... vs) {
    if constexpr (not internal::HasValueStack<I>) {
      constexpr size_t NumberOfArgumentsProvidedToAppend = sizeof...(vs);
      constexpr size_t NumberOfImmediateValuesRequiredByInstruction =
          internal::ImmediateValueCount<I>();
      static_assert(
          // clang-format off
          NumberOfArgumentsProvidedToAppend == NumberOfImmediateValuesRequiredByInstruction,
          // clang-format on
          "Incorrect number of immediate values provided to `append`.");
      return internal::FunctionBase::append({ExecPtr<I>(), Value(vs)...});
    } else {
      constexpr size_t DropCount = internal::HasValueStack<I> +
                                   HasExecutionState<I> +
                                   internal::HasFunctionState<I>;
      constexpr auto parameters = nth::type<decltype(I::execute)>.parameters();
      return parameters.template drop<DropCount>().reduce(
          [&](auto... argument_types) {
            return internal::FunctionBase::append(
                {ExecPtr<I>(), Value(argument_types.cast(vs))...});
          });
    }
  }

  // Appends an instruction followed by space for `placeholder_count` values
  // which are left uninitialized. They may be initialized later via calls to
  // `Function<...>::set_value`. Returns the corresponding
  // `nth::interval<InstructionIndex>`.
  template <internal::ContainedIn<Set> I>
  constexpr nth::interval<InstructionIndex> append_with_placeholders() {
    return internal::FunctionBase::append(ExecPtr<I>(),
                                          internal::ImmediateValueCount<I>());
  }
};

}  // namespace jasmin

#endif  // JASMIN_FUNCTION_H
