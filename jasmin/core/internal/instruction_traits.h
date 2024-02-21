#ifndef JASMIN_CORE_INTERNAL_INSTRUCTION_TRAITS_H
#define JASMIN_CORE_INTERNAL_INSTRUCTION_TRAITS_H

#include "jasmin/core/input.h"
#include "jasmin/core/internal/function_state.h"
#include "jasmin/core/output.h"
#include "jasmin/core/value.h"
#include "nth/io/deserialize/deserialize.h"
#include "nth/io/serialize/serialize.h"
#include "nth/meta/concepts.h"
#include "nth/meta/type.h"

namespace jasmin {

// Forward declarations for instructions that need special treatement in
// Jasmin's interpreter and are built-in to every instruction set.
struct Call;
struct Jump;
struct JumpIf;
struct JumpIfNot;
struct Return;

struct InstructionSpecification {
  uint32_t parameters;
  uint32_t returns;

  template <typename S>
  friend nth::io::serializer_result_type<S>NthSerialize(S& s, InstructionSpecification spec) {
    return nth::io::serializer_result_type<S>(
        nth::io::write_integer(s, spec.parameters) and
        nth::io::write_integer(s, spec.returns));
  }

  template <typename D>
  friend nth::io::deserializer_result_type<D> NthDeserialize(
      D& d, InstructionSpecification& spec) {
    return nth::io::deserializer_result_type<D>(
        nth::io::read_integer(d, spec.parameters) and
        nth::io::read_integer(d, spec.returns));
  }
};

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
  return nth::any_of<I, Call, Jump, JumpIf, JumpIfNot, Return>;
}

template <typename I>
constexpr bool HasExactlyOneOfConsumeOrExecute() {
  return (
      requires { &I::consume; } or requires { &I::execute; });
}

constexpr bool ImmediateValueDetermined(nth::Sequence auto seq) {
  if constexpr (seq.size() < 2) {
    return false;
  } else {
    return seq.template get<0>() == nth::type<std::span<Value>> and
           seq.template get<1>() == nth::type<std::span<Value>>;
  }
}

constexpr bool ValidParameterSequence(nth::Sequence auto seq) {
  if constexpr (seq.size() < 2) {
    return false;
  } else {
    if constexpr ((std::derived_from<nth::type_t<seq.template get<0>()>,
                                     internal::InputBase> and
                   std::derived_from<nth::type_t<seq.template get<1>()>,
                                     internal::OutputBase>) or
                  (seq.template get<0>() == nth::type<std::span<Value>> and
                   seq.template get<1>() == nth::type<std::span<Value>>)) {
      return seq.template drop<1>().template all<[](auto t) {
        constexpr auto underlying = t.without_reference().without_const();
        if constexpr (std::is_trivially_copyable_v<nth::type_t<underlying>> and
                      underlying.alignment() <= alignof(Value)) {
          using U = nth::type_t<underlying>;
          return nth::type<U> == t or nth::type<U const &> == t;
        } else {
          return false;
        }
      }>();
    } else {
      return false;
    }
  }
}

template <typename I>
constexpr auto InstructionFunctionType() {
  if constexpr (nth::type<I> == nth::type<Call>) {
    return nth::type<void(std::span<Value, 1>, InstructionSpecification)>;
  } else if constexpr (nth::type<I> == nth::type<Jump>) {
    return nth::type<void(std::span<Value, 0>, ptrdiff_t)>;
  } else if constexpr (nth::type<I> == nth::type<JumpIf>) {
    return nth::type<void(std::span<Value, 1>, ptrdiff_t)>;
  } else if constexpr (nth::type<I> == nth::type<JumpIfNot>) {
    return nth::type<void(std::span<Value, 1>, ptrdiff_t)>;
  } else if constexpr (nth::type<I> == nth::type<Return>) {
    return nth::type<void(std::span<Value, 0>)>;
  } else {
    if constexpr (requires { &I::consume; }) {
      static_assert(std::is_function_v<decltype(I::consume)>);
      return nth::type<decltype(I::consume)>;
    } else {
      static_assert(std::is_function_v<decltype(I::execute)>);
      return nth::type<decltype(I::execute)>;
    }
  }
}

template <typename I>
constexpr auto InstructionFunctionPointer() {
  if constexpr (requires { &I::consume; }) {
    return &I::consume;
  } else {
    return &I::execute;
  }
}

template <typename I>
constexpr bool ValidSignatureWithFunctionState() {
  auto body_type = InstructionFunctionType<I>();
  if constexpr (body_type.return_type() == nth::type<void>) {
    return ImmediateValueDetermined(
               body_type.parameters().template drop<1>()) or
           ValidParameterSequence(body_type.parameters().template drop<1>());
  } else {
    return false;
  }
}

template <typename I>
constexpr bool ValidSignatureWithoutFunctionState() {
  auto body_type = InstructionFunctionType<I>();
  if constexpr (body_type.return_type() == nth::type<void>) {
    return ImmediateValueDetermined(body_type.parameters()) or
           ValidParameterSequence(body_type.parameters());
  } else {
    return false;
  }
}

template <typename I>
concept UserDefinedInstruction = HasExactlyOneOfConsumeOrExecute<I>() and
    ((HasFunctionState<I> and ValidSignatureWithFunctionState<I>()) or
     (not HasFunctionState<I> and ValidSignatureWithoutFunctionState<I>()));

constexpr size_t RoundUp(size_t n, size_t d) { return n / d + (n % d != 0); }

template <typename I>
constexpr size_t ImmediateValueBytes() {
  return InstructionFunctionType<I>()
      .parameters()
      .template drop<(HasFunctionState<I> ? 2 : 3)>()
      .reduce([](auto... ts) {
        return (0 + ... + RoundUp(ts.size(), sizeof(Value)));
      });
}

}  // namespace internal
}  // namespace jasmin

#endif  // JASMIN_CORE_INTERNAL_INSTRUCTION_TRAITS_H
