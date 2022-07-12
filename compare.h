#ifndef JASMIN_COMPARE_H
#define JASMIN_COMPARE_H

#include <string_view>

#include "jasmin/execute.h"

namespace jasmin {

template <typename T>
concept Comparable = requires(T t) {
  { t < t } -> std::same_as<bool>;
};

template <Comparable T>
struct LessThan : StackMachineInstruction<LessThan<T>> {
  static constexpr std::string_view jasmin_instruction = "lt";
  static constexpr bool execute(T x, T y) { return x < y; }
};

template <typename T>
concept Equatable = requires(T t) {
  { t == t } -> std::same_as<bool>;
};

template <Equatable T>
struct Equal : StackMachineInstruction<Equal<T>> {
  static constexpr std::string_view jasmin_instruction = "eq";
  static constexpr bool execute(T x, T y) { return x == y; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_H
