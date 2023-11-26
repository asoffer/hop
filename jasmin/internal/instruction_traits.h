#ifndef JASMIN_INTERNAL_INSTRUCTION_TRAITS_H
#define JASMIN_INTERNAL_INSTRUCTION_TRAITS_H

#include "jasmin/internal/function_state.h"
#include "jasmin/value.h"
#include "nth/meta/concepts.h"
#include "nth/meta/type.h"

namespace jasmin {

// Forward declarations for instructions that need special treatement in
// Jasmin's interpreter and are built-in to every instruction set.
struct Call;
struct Jump;
struct JumpIf;
struct Return;

namespace internal {

// Base class used solely to indicate that any struct inherting from it is an
// instruction set.
struct InstructionSetBase {};

// Returns true if and only if `I` is a builtin instruction. Using a function
// rather than a concept so that error messages expanding the concept do not
// expand past this. Seeing the list of builtin instructions is more likely to
// be noise than to be valuable for most users.
template <typename I>
constexpr bool BuiltinInstruction() {
  return nth::any_of<I, Call, Jump, JumpIf, Return>;
}

template <typename I>
constexpr bool HasExactlyOneOfConsumeOrExecute() {
  return (
      requires { &I::consume; } or requires { &I::execute; });
}

constexpr bool ValidParameterSequence(nth::Sequence auto seq) {
  if constexpr (seq.empty()) {
    return false;
  } else {
    using span_type = nth::type_t<seq.template get<0>()>;
    if constexpr (requires {
                    { span_type::extent } -> std::same_as<size_t const>;
                  }) {
      return false;
    } else if constexpr (seq.template get<0>() !=
                         nth::type<std::span<Value, span_type::extent>>) {
      return false;
    } else {
      return seq.template drop<1>().template all<[](auto t) {
        return std::is_convertible_v<nth::type_t<t>, Value>;
      }>();
    }
  }
}

constexpr bool ValidSignatureWithFunctionStateImpl(
    nth::Type auto body_type, nth::Type auto /*function_state_type*/) {
  if constexpr (body_type.return_type() == nth::type<void>) {
    return ValidParameterSequence(body_type.parameters().template drop<1>());
  } else if constexpr (std::convertible_to<nth::type_t<body_type>,
                                           jasmin::Value>) {
    return ValidParameterSequence(body_type.parameters().template drop<1>());
  } else {
    return false;
  }
}

constexpr bool ValidSignatureWithoutFunctionStateImpl(
    nth::Type auto body_type) {
  if constexpr (body_type.return_type() == nth::type<void>) {
    return ValidParameterSequence(body_type.parameters());
  } else if constexpr (std::convertible_to<nth::type_t<body_type>,
                                           jasmin::Value>) {
    return ValidParameterSequence(body_type.parameters());
  } else {
    return false;
  }
}

template <typename I>
constexpr bool ValidSignatureWithFunctionState() {
  if constexpr (requires { &I::execute; }) {
    return std::is_function_v<decltype(I::execute)> and
           ValidSignatureWithFunctionStateImpl(
               nth::type<decltype(I::execute)>,
               nth::type<typename I::function_state>);
  } else {
    return std::is_function_v<decltype(I::consume)> and
           ValidSignatureWithFunctionStateImpl(
               nth::type<decltype(I::consume)>,
               nth::type<typename I::function_state>);
  }
}

template <typename I>
constexpr bool ValidSignatureWithoutFunctionState() {
  if constexpr (requires { &I::execute; }) {
    return std::is_function_v<decltype(I::execute)> and
           ValidSignatureWithoutFunctionStateImpl(
               nth::type<decltype(I::execute)>);
  } else {
    return std::is_function_v<decltype(I::consume)> and
           ValidSignatureWithoutFunctionStateImpl(
               nth::type<decltype(I::consume)>);
  }
}

template <typename I>
concept UserDefinedInstruction = HasExactlyOneOfConsumeOrExecute<I>() and
    ((HasFunctionState<I> and ValidSignatureWithFunctionState<I>()) or
     (not HasFunctionState<I> and ValidSignatureWithoutFunctionState<I>()));

template <typename I>
constexpr auto InstructionFunctionPointer() {
  if constexpr (requires { &I::consume; }) {
    return &I::consume;
  } else {
    return &I::execute;
  }
}

template <typename I>
constexpr auto InstructionFunctionType() {
  return nth::type<decltype(*InstructionFunctionPointer<I>())>.without_reference();
}

}  // namespace internal
}  // namespace jasmin

#endif  // JASMIN_INTERNAL_INSTRUCTION_TRAITS_H
