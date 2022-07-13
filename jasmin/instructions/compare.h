#ifndef JASMIN_INSTRUCTIONS_COMPARE_H
#define JASMIN_INSTRUCTIONS_COMPARE_H

#include <concepts>

#include "jasmin/instruction.h"

namespace jasmin {

template <typename T>
concept Comparable = requires(T t) {
  { t < t } -> std::same_as<bool>;
};

template <Comparable T>
struct LessThan : StackMachineInstruction<LessThan<T>> {
  static constexpr bool execute(T x, T y) { return x < y; }
};

template <typename T>
concept Equatable = requires(T t) {
  { t == t } -> std::same_as<bool>;
};

template <Equatable T>
struct Equal : StackMachineInstruction<Equal<T>> {
  static constexpr bool execute(T x, T y) { return x == y; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_COMPARE_H
