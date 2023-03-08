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
  static constexpr std::string_view debug() { return "less-than"; }
};

template <Comparable T>
struct AppendLessThan : StackMachineInstruction<AppendLessThan<T>> {
  static constexpr void execute(ValueStack &value_stack) {
    value_stack.push(value_stack.peek<T>(1) < value_stack.peek<T>(0));
  }
  static constexpr std::string_view debug() { return "append less-than"; }
};

template <typename T>
concept Equatable = requires(T t) {
  { t == t } -> std::same_as<bool>;
};

template <Equatable T>
struct Equal : StackMachineInstruction<Equal<T>> {
  static constexpr bool execute(T x, T y) { return x == y; }
  static constexpr std::string_view debug() { return "equal"; }
};

template <Equatable T>
struct AppendEqual : StackMachineInstruction<AppendEqual<T>> {
  static constexpr void execute(ValueStack &value_stack) {
    value_stack.push(value_stack.peek<T>(1) == value_stack.peek<T>(0));
  }
  static constexpr std::string_view debug() { return "append equal"; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_COMPARE_H
