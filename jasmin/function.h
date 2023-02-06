#ifndef JASMIN_FUNCTION_H
#define JASMIN_FUNCTION_H

#include <type_traits>

#include "jasmin/call_stack.h"
#include "jasmin/instruction.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/internal/debug.h"
#include "jasmin/internal/function_base.h"
#include "jasmin/internal/type_traits.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {

// A representation of a function that ties op-codes to instructions (via an
// InstructionSet template parameter).
template <InstructionSet Set>
struct Function : internal::FunctionBase {
 public:
  // Constructs an empty `Function` given a `parameter_count` representing
  // the number of parameters to the function, and a `return_count`
  // representing the number of return values for the function.
  explicit constexpr Function(uint8_t parameter_count, uint8_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  // Appends an op-code for the given `Instruction I` template parameter.
  template <typename I>
  requires(
      Set::instructions.template contains<nth::type<I>>()) constexpr OpCodeRange
      append(auto... vs) {
    if constexpr (std::is_same_v<I, Return> or std::is_same_v<I, Call>) {
      return internal::FunctionBase::append(
          {Value(&I::template ExecuteImpl<typename Set::self_type>)});
    } else {
      using signature = internal::ExtractSignature<decltype(&I::execute)>;
      if constexpr (std::is_same_v<I, Return> or std::is_same_v<I, Call> or
                    not internal::HasValueStack<signature>) {
        return internal::FunctionBase::append(
            {Value(&I::template ExecuteImpl<typename Set::self_type>)});
      } else {
        constexpr size_t DropCount = internal::HasValueStack<signature> +
                                     HasExecutionState<I> + HasFunctionState<I>;

        if constexpr (DropCount == 0) {
          return internal::ExtractSignature<decltype(&I::execute)>::
              invoke_with_argument_types([&]<typename... Arguments>() {
                return internal::FunctionBase::append(
                    {Value(&I::template ExecuteImpl<typename Set::self_type>),
                     Value(static_cast<Arguments>(vs))...});
              });
        } else if constexpr (DropCount == 1) {
          return internal::ExtractSignature<decltype(&I::execute)>::
              invoke_with_argument_types(
                  [&]<typename, typename... Arguments>() {
                    return internal::FunctionBase::append(
                        {Value(
                             &I::template ExecuteImpl<typename Set::self_type>),
                         Value(static_cast<Arguments>(vs))...});
                  });
        } else if constexpr (DropCount == 2) {
          return internal::ExtractSignature<decltype(&I::execute)>::
              invoke_with_argument_types(
                  [&]<typename, typename, typename... Arguments>() {
                    return internal::FunctionBase::append(
                        {Value(
                             &I::template ExecuteImpl<typename Set::self_type>),
                         Value(static_cast<Arguments>(vs))...});
                  });
        } else if constexpr (DropCount == 3) {
          return internal::ExtractSignature<decltype(&I::execute)>::
              invoke_with_argument_types(
                  [&]<typename, typename, typename, typename... Arguments>() {
                    return internal::FunctionBase::append(
                        {Value(
                             &I::template ExecuteImpl<typename Set::self_type>),
                         Value(static_cast<Arguments>(vs))...});
                  });
        }
      }
    }
  }

  // Appends an intsruction followed by space for `placeholder_count` values
  // which are left uninitialized. They may be initialized later via calls to
  // `Function<...>::set_value`. Returns the corresponding OpCodeRange.
  template <typename I>
  requires(
      Set::instructions.template contains<nth::type<I>>()) constexpr OpCodeRange
      append_with_placeholders() {
    return internal::FunctionBase::append(
        Value(&I::template ExecuteImpl<typename Set::self_type>),
        internal::ImmediateValueCount<I>());
  }
};

}  // namespace jasmin

#endif  // JASMIN_FUNCTION_H
