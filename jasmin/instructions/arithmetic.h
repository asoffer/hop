#ifndef JASMIN_INSTRUCTIONS_ARITHMETIC_H
#define JASMIN_INSTRUCTIONS_ARITHMETIC_H

#include <concepts>

#include "jasmin/core/instruction.h"

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
struct Add : Instruction<Add<T>> {
  static constexpr void consume(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = T(values[0].as<T>() + values[1].as<T>());
  }
};

template <Subtractable T>
struct Subtract : Instruction<Subtract<T>> {
  static constexpr void consume(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = T(values[0].as<T>() - values[1].as<T>());
  }
};

template <Multiplicable T>
struct Multiply : Instruction<Multiply<T>> {
  static constexpr void consume(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = T(values[0].as<T>() * values[1].as<T>());
  }
};

template <Divisible T>
struct Divide : Instruction<Divide<T>> {
  static constexpr void consume(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = T(values[0].as<T>() / values[1].as<T>());
  }
};

template <Modable T>
struct Mod : Instruction<Mod<T>> {
  static constexpr void consume(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = T(values[0].as<T>() % values[1].as<T>());
  }
};

template <Negatable T>
struct Negate : Instruction<Negate<T>> {
  static constexpr void execute(std::span<Value, 1> values,
                                std::span<Value, 0>) {
    values[0] = T(-values[0].as<T>());
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_ARITHMETIC_H
