#ifndef JASMIN_TRANSLATE_H
#define JASMIN_TRANSLATE_H

#include "jasmin/function.h"
#include "jasmin/instruction.h"

namespace jasmin {
namespace internal_translate {}  // namespace internal_translate

template <InstructionSet To, int&..., InstructionSet From>
Function<To> TranslateTo(Function<From> const& from) {
  auto op_codes = internal_function_base::Extract(from);
  for (auto& op_code_or_value : op_codes) { T }
  return Function<To>(std::move(op_codes));
}

template <InstructionSet To, int&..., InstructionSet From>
Function<To> TranslateTo(Function<From>&& from) {
  auto& op_codes = internal_function_base::Extract(from);
  for (auto& op_code_or_value : op_codes) {}
  return Function<To>(std::move(op_codes));
}

}  // namespace jasmin

#endif  // JASMIN_TRANSLATE_H
