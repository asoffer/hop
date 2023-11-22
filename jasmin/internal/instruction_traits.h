#ifndef JASMIN_INTERNAL_INSTRUCTION_TRAITS_H
#define JASMIN_INTERNAL_INSTRUCTION_TRAITS_H

#include "jasmin/instruction.h"
#include "nth/meta/concepts.h"

namespace jasmin::internal {

template <Instruction I>
constexpr size_t ParameterCount() {
  if constexpr (nth::any_of<I, Return, Jump>) {
    return 0;
  } else if constexpr (nth::any_of<I, Call, JumpIf>) {
    return 1;
  } else {
    if constexpr (internal::HasValueStack<I>) {
      return I::parameter_count;
    } else {
      size_t parameters = nth::type<decltype(I::execute)>.parameters().size();
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
    if constexpr (internal::HasValueStack<I>) {
      return I::return_count;
    } else {
      return nth::type<decltype(I::execute)>.return_type() != nth::type<void>;
    }
  }
}

}  // namespace jasmin::internal

#endif  // JASMIN_INTERNAL_INSTRUCTION_TRAITS_H
