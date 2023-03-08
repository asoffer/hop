#ifndef JASMIN_INSTRUCTIONS_ARITHMETIC_H
#define JASMIN_INSTRUCTIONS_ARITHMETIC_H

#include <concepts>

#include "jasmin/instruction.h"

namespace jasmin {

template <typename T>
concept Addable = not std::same_as<bool, T> and requires(T t) {
  { t + t } -> std::convertible_to<T>;
};

template <typename T>
concept Subtractable = not std::same_as<bool, T> and requires(T t) {
  { t - t } -> std::convertible_to<T>;
};

template <typename T>
concept Multiplicable = not std::same_as<bool, T> and requires(T t) {
  { (t * t) } -> std::convertible_to<T>;
};

template <typename T>
concept Divisible = not std::same_as<bool, T> and requires(T t) {
  { t / t } -> std::convertible_to<T>;
};

template <typename T>
concept Modable = not std::same_as<bool, T> and requires(T t) {
  { t % t } -> std::convertible_to<T>;
};

template <typename T>
concept Negatable = not std::same_as<bool, T> and requires(T t) {
  { -t } -> std::convertible_to<T>;
};

template <Addable T>
struct Add : StackMachineInstruction<Add<T>> {
  static constexpr T execute(T x, T y) { return x + y; }
  static constexpr std::string_view debug() { return "add"; }
};

template <Subtractable T>
struct Subtract : StackMachineInstruction<Subtract<T>> {
  static constexpr T execute(T x, T y) { return x - y; }
  static constexpr std::string_view debug() { return "sub"; }
};

template <Multiplicable T>
struct Multiply : StackMachineInstruction<Multiply<T>> {
  static constexpr T execute(T x, T y) { return x * y; }
  static constexpr std::string_view debug() { return "mul"; }
};

template <Divisible T>
struct Divide : StackMachineInstruction<Divide<T>> {
  static constexpr T execute(T x, T y) { return x / y; }
  static constexpr std::string_view debug() { return "div"; }
};

template <Modable T>
struct Mod : StackMachineInstruction<Mod<T>> {
  static constexpr T execute(T x, T y) { return x % y; }
  static constexpr std::string_view debug() { return "mod"; }
};

template <Negatable T>
struct Negate : StackMachineInstruction<Negate<T>> {
  static constexpr T execute(T x) { return -x; }
  static constexpr std::string_view debug() { return "negate"; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_ARITHMETIC_H
