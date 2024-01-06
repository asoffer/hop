#ifndef JASMIN_CORE_FUNCTION_H
#define JASMIN_CORE_FUNCTION_H

#include "jasmin/core/instruction.h"
#include "jasmin/core/instruction_index.h"
#include "jasmin/core/internal/function_base.h"
#include "jasmin/core/internal/instruction_traits.h"
#include "nth/container/interval.h"
#include "nth/meta/type.h"

namespace jasmin {

// A representation of a function that ties op-codes to instructions (via an
// InstructionSet template parameter).
template <InstructionSetType Set>
struct Function : internal::FunctionBase {
  using instruction_set = Set;

  // Constructs an empty `Function` given a `parameter_count` representing
  // the number of parameters to the function, and a `return_count`
  // representing the number of return values for the function.
  explicit constexpr Function(uint32_t parameter_count, uint32_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  // Appends an op-code for the given instruction `I` template parameter.
  template <typename I>
  requires(instruction_set::instructions.template contains<nth::type<I>>())  //
      constexpr nth::interval<InstructionIndex> append(auto... vs);

  // Appends an op-code for the given instruction `I` template parameter.
  template <typename I>
  requires(instruction_set::instructions.template contains<nth::type<I>>())  //
      constexpr nth::interval<InstructionIndex> append(
          InstructionSpecification spec, auto... vs);

  // Appends an instruction followed by space for `placeholder_count` values
  // which are left uninitialized. They may be initialized later via calls to
  // `Function<...>::set_value`. Returns the corresponding
  // `nth::interval<InstructionIndex>`.
  template <typename I>
  requires(instruction_set::instructions.template contains<nth::type<I>>())  //
      constexpr nth::interval<InstructionIndex> append_with_placeholders();
};

template <InstructionSetType Set>
template <typename I>
requires(Set::instructions.template contains<nth::type<I>>())  //
    constexpr nth::interval<InstructionIndex> Function<Set>::append(
        auto... vs) {
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

template <InstructionSetType Set>
template <typename I>
requires(Set::instructions.template contains<nth::type<I>>())  //
    constexpr nth::interval<InstructionIndex> Function<Set>::append(
        InstructionSpecification spec, auto... vs) {
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

template <InstructionSetType Set>
template <typename I>
requires(Set::instructions.template contains<nth::type<I>>())  //
    constexpr nth::interval<InstructionIndex> Function<
        Set>::append_with_placeholders() {
  return internal::FunctionBase::append(&I::template ExecuteImpl<Set>,
                                        ImmediateValueCount<I>());
}

}  // namespace jasmin

#endif  // JASMIN_CORE_FUNCTION_H
