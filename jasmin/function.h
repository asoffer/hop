#ifndef JASMIN_FUNCTION_H
#define JASMIN_FUNCTION_H

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
template <InstructionSetType Set>
struct Function : internal::FunctionBase {
  // Constructs an empty `Function` given a `parameter_count` representing
  // the number of parameters to the function, and a `return_count`
  // representing the number of return values for the function.
  explicit constexpr Function(uint8_t parameter_count, uint8_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  // Appends an op-code for the given instruction `I` template parameter.
  template <internal::ContainedIn<Set> I>
  constexpr nth::interval<InstructionIndex> append(auto... vs) {
    constexpr size_t DropCount = internal::HasFunctionState<I> ? 2 : 1;
    return internal::InstructionFunctionType<I>()
        .parameters()
        .template drop<DropCount>()
        .reduce([&](auto... ts) {
          return internal::FunctionBase::append(
              {&I::template ExecuteImpl<Set>,
               Value(static_cast<nth::type_t<ts>>(vs))...});
        });
  }
  template <internal::ContainedIn<Set> I>
  constexpr nth::interval<InstructionIndex> append(InstructionSpecification spec, auto... vs) {
    constexpr size_t DropCount = internal::HasFunctionState<I> ? 3 : 2;
    return internal::InstructionFunctionType<I>()
        .parameters()
        .template drop<DropCount>()
        .reduce([&](auto... ts) {
          return internal::FunctionBase::append(
              {&I::template ExecuteImpl<Set>, spec,
               Value(static_cast<nth::type_t<ts>>(vs))...});
        });
  }


  // Appends an instruction followed by space for `placeholder_count` values
  // which are left uninitialized. They may be initialized later via calls to
  // `Function<...>::set_value`. Returns the corresponding
  // `nth::interval<InstructionIndex>`.
  template <internal::ContainedIn<Set> I>
  constexpr nth::interval<InstructionIndex> append_with_placeholders() {
    return internal::FunctionBase::append(&I::template ExecuteImpl<Set>,
                                          ImmediateValueCount<I>());
  }
};

}  // namespace jasmin

#endif  // JASMIN_FUNCTION_H
