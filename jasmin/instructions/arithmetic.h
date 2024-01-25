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
  static constexpr void consume(Input<T, T> in, Output<T> out) {
    out.template set<0>(in.template get<0>() + in.template get<1>());
  }
};

template <Subtractable T>
struct Subtract : Instruction<Subtract<T>> {
  static constexpr void consume(Input<T, T> in, Output<T> out) {
    out.template set<0>(in.template get<0>() - in.template get<1>());
  }
};

template <Multiplicable T>
struct Multiply : Instruction<Multiply<T>> {
  static constexpr void consume(Input<T, T> in, Output<T> out) {
    out.template set<0>(in.template get<0>() * in.template get<1>());
  }
};

template <Divisible T>
struct Divide : Instruction<Divide<T>> {
  static constexpr void consume(Input<T, T> in, Output<T> out) {
    out.template set<0>(in.template get<0>() / in.template get<1>());
  }
};

template <Modable T>
struct Mod : Instruction<Mod<T>> {
  static constexpr void consume(Input<T, T> in, Output<T> out) {
    out.template set<0>(in.template get<0>() % in.template get<1>());
  }
};

template <Negatable T>
struct Negate : Instruction<Negate<T>> {
  static constexpr void consume(Input<T> in, Output<T> out) {
    out.template set<0>(-in.template get<0>());
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_ARITHMETIC_H
