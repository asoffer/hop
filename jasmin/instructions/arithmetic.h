#ifndef JASMIN_INSTRUCTIONS_ARITHMETIC_H
#define JASMIN_INSTRUCTIONS_ARITHMETIC_H

#include <concepts>

#include "jasmin/instruction.h"

namespace jasmin {

template <typename T>
concept Addable = requires(T t) {
  { t + t } -> std::convertible_to<T>;
};

template <typename T>
concept Subtractable = requires(T t) {
  { t - t } -> std::convertible_to<T>;
};

template <typename T>
concept Multiplicable = requires(T t) {
  { t *t } -> std::convertible_to<T>;
};

template <typename T>
concept Divisible = requires(T t) {
  { t / t } -> std::convertible_to<T>;
};

template <typename T>
concept Modable = requires(T t) {
  { t % t } -> std::convertible_to<T>;
};

template <typename T>
concept Negatable = requires(T t) {
  { -t } -> std::convertible_to<T>;
};

template <Addable T>
struct Add : StackMachineInstruction<Add<T>> {
  static constexpr T execute(T x, T y) { return x + y; }
};

template <Subtractable T>
struct Subtract : StackMachineInstruction<Subtract<T>> {
  static constexpr T execute(T x, T y) { return x - y; }
};

template <Multiplicable T>
struct Multiply : StackMachineInstruction<Multiply<T>> {
  static constexpr T execute(T x, T y) { return x * y; }
};

template <Divisible T>
struct Divide : StackMachineInstruction<Divide<T>> {
  static constexpr T execute(T x, T y) { return x / y; }
};

template <Modable T>
struct Mod : StackMachineInstruction<Mod<T>> {
  static constexpr T execute(T x, T y) { return x % y; }
};

template <Modable T>
struct Negate : StackMachineInstruction<Negate<T>> {
  static constexpr T execute(T x) { return -x; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_ARITHMETIC_H
