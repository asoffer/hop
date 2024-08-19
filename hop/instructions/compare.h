#ifndef JASMIN_INSTRUCTIONS_COMPARE_H
#define JASMIN_INSTRUCTIONS_COMPARE_H

#include <concepts>

#include "hop/core/instruction.h"

namespace hop {

template <typename T>
concept Comparable = requires(T t) {
  { t < t } -> std::same_as<bool>;
};

template <Comparable T>
struct LessThan : Instruction<LessThan<T>> {
  static constexpr void consume(Input<T, T> in, Output<bool> out) {
    out.set<0>(in.template get<0>() < in.template get<1>());
  }
};

template <Comparable T>
struct AppendLessThan : Instruction<AppendLessThan<T>> {
  static constexpr void execute(Input<T, T> in, Output<bool> out) {
    out.set<0>(in.template get<0>() < in.template get<1>());
  }
};

template <typename T>
concept Equatable = requires(T t) {
  { t == t } -> std::same_as<bool>;
};

template <Equatable T>
struct Equal : Instruction<Equal<T>> {
  static constexpr void consume(Input<T, T> in, Output<bool> out) {
    out.set<0>(in.template get<0>() == in.template get<1>());
  }
};

template <Equatable T>
struct AppendEqual : Instruction<AppendEqual<T>> {
  static constexpr void execute(Input<T, T> in, Output<bool> out) {
    out.set<0>(in.template get<0>() == in.template get<1>());
  }
};

}  // namespace hop

#endif  // JASMIN_INSTRUCTIONS_COMPARE_H
