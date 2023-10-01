#ifndef JASMIN_INTERNAL_INSTRUCTION_TRAITS_H
#define JASMIN_INTERNAL_INSTRUCTION_TRAITS_H

#include "jasmin/instruction.h"
#include "nth/meta/concepts.h"

namespace jasmin::internal {

template <Instruction I>
constexpr size_t ImmediateValueCount() {
  if constexpr (nth::any_of<I, Call, Return>) {
    return 0;
  } else if constexpr (nth::any_of<I, Jump, JumpIf>) {
    return 1;
  } else {
    using signature = ExtractSignature<decltype(&I::execute)>;

    size_t immediate_value_count = signature::invoke_with_argument_types(
        []<typename... Ts>() { return sizeof...(Ts); });

    constexpr bool ES = HasExecutionState<I>;
    constexpr bool FS = internal::HasFunctionState<I>;
    constexpr bool VS = internal::HasValueStack<I>;

    if (not VS) { return 0; }
    --immediate_value_count;  // Ignore the `ValueStack&` parameter.
    if (ES) { --immediate_value_count; }
    if (FS) { --immediate_value_count; }
    return immediate_value_count;
  }
}

template <Instruction I>
constexpr size_t ParameterCount() {
  if constexpr (nth::any_of<I, Return, Jump>) {
    return 0;
  } else if constexpr (nth::any_of<I, Call, JumpIf>) {
    return 1;
  } else {
    using signature = ExtractSignature<decltype(&I::execute)>;

    if constexpr (internal::HasValueStack<I>) {
      return I::parameter_count;
    } else {
      size_t parameters = signature::invoke_with_argument_types(
          []<typename... Ts>() { return sizeof...(Ts); });

      if (HasExecutionState<I>) { --parameters; }
      if (internal::HasFunctionState<I>) { --parameters; }
      return parameters;
    }
  }
}

template <Instruction I>
constexpr size_t ReturnCount() {
  if constexpr (nth::any_of<I, Return, Call, Jump, JumpIf>) {
    return 0;
  } else {
    using signature = ExtractSignature<decltype(&I::execute)>;

    if constexpr (internal::HasValueStack<I>) {
      return I::return_count;
    } else {
      return nth::type<typename signature::return_type> != nth::type<void>;
    }
  }
}

}  // namespace jasmin::internal

#endif  // JASMIN_INTERNAL_INSTRUCTION_TRAITS_H
