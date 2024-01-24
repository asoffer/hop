#ifndef JASMIN_INSTRUCTIONS_COMPARE_H
#define JASMIN_INSTRUCTIONS_COMPARE_H

#include <concepts>

#include "jasmin/core/instruction.h"

namespace jasmin {

template <typename T>
concept Comparable = requires(T t) {
  { t < t } -> std::same_as<bool>;
};

template <Comparable T>
struct LessThan : Instruction<LessThan<T>> {
  static constexpr void consume(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = values[0].as<T>() < values[1].as<T>();
  }
};

template <Comparable T>
struct AppendLessThan : Instruction<AppendLessThan<T>> {
  static constexpr void execute(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = values[0].as<T>() < values[1].as<T>();
  }
};

template <typename T>
concept Equatable = requires(T t) {
  { t == t } -> std::same_as<bool>;
};

template <Equatable T>
struct Equal : Instruction<Equal<T>> {
  static constexpr void consume(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = values[0].as<T>() == values[1].as<T>();
  }
};

template <Equatable T>
struct AppendEqual : Instruction<AppendEqual<T>> {
  static constexpr void execute(std::span<Value, 2> values,
                                std::span<Value, 1> out) {
    out[0] = values[0].as<T>() == values[1].as<T>();
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_COMPARE_H
