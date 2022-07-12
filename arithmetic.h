#ifndef JASMIN_ARITHMETIC_H
#define JASMIN_ARITHMETIC_H

#include <concepts>
#include <string_view>

#include "jasmin/execute.h"

namespace jasmin {

template <typename T>
concept Addable = requires(T t) {
  { t - t } -> std::same_as<T>;
};

template <typename T>
concept Subtractable = requires(T t) {
  { t - t } -> std::same_as<T>;
};

template <typename T>
concept Multiplicable = requires(T t) {
  { t *t } -> std::same_as<T>;
};

template <typename T>
concept Divisible = requires(T t) {
  { t / t } -> std::same_as<T>;
};

template <typename T>
concept Modable = requires(T t) {
  { t % t } -> std::same_as<T>;
};

template <Addable T>
struct Add : StackMachineInstruction<Add<T>> {
  static constexpr std::string_view jasmin_instruction = "add";
  static constexpr T execute(T x, T y) { return x + y; }
};

template <Subtractable T>
struct Subtract : StackMachineInstruction<Subtract<T>> {
  static constexpr std::string_view jasmin_instruction = "sub";
  static constexpr T execute(T x, T y) { return x - y; }
};

template <Multiplicable T>
struct Multiply : StackMachineInstruction<Multiply<T>> {
  static constexpr std::string_view jasmin_instruction = "mul";
  static constexpr T execute(T x, T y) { return x * y; }
};

template <Divisible T>
struct Divide : StackMachineInstruction<Divide<T>> {
  static constexpr std::string_view jasmin_instruction = "div";
  static constexpr T execute(T x, T y) { return x / y; }
};

template <Modable T>
struct Mod : StackMachineInstruction<Mod<T>> {
  static constexpr std::string_view jasmin_instruction = "mod";
  static constexpr T execute(T x, T y) { return x % y; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_H
