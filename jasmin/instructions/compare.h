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
  static constexpr bool consume(std::span<Value, 2> values) {
    return values[0].as<T>() < values[1].as<T>();
  }
  static constexpr std::string_view debug() { return "less-than"; }
};

template <Comparable T>
struct AppendLessThan : Instruction<AppendLessThan<T>> {
  static constexpr bool execute(std::span<Value, 2> values) {
    return values[0].as<T>() < values[1].as<T>();
  }
  static constexpr std::string_view debug() { return "append less-than"; }
};

template <typename T>
concept Equatable = requires(T t) {
  { t == t } -> std::same_as<bool>;
};

template <Equatable T>
struct Equal : Instruction<Equal<T>> {
  static constexpr bool consume(std::span<Value, 2> values) {
    return values[0].as<T>() == values[1].as<T>();
  }
  static constexpr std::string_view debug() { return "equal"; }
};

template <Equatable T>
struct AppendEqual : Instruction<AppendEqual<T>> {
  static constexpr bool execute(std::span<Value, 2> values) {
    return values[0].as<T>() == values[1].as<T>();
  }
  static constexpr std::string_view debug() { return "append equal"; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_COMPARE_H
