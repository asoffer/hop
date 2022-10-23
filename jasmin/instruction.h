#ifndef JASMIN_INSTRUCTION_H
#define JASMIN_INSTRUCTION_H

#include <concepts>
#include <limits>
#include <stack>
#include <type_traits>

#include "jasmin/call_stack.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/internal/attributes.h"
#include "jasmin/internal/function_base.h"
#include "jasmin/internal/type_list.h"
#include "jasmin/internal/type_traits.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"

namespace jasmin {
namespace internal {

using exec_fn_type = void (*)(ValueStack &, InstructionPointer &, CallStack &,
                              void *);

// Base class used solely to indicate that any struct inherting from it is an
// instruction set.
struct InstructionSetBase {};

// Implementation detail. A concept capturing valid return types for one way a
// type can adhere to the `Instruction` concept defined below.
template <typename T>
concept VoidOrConvertibleToValue =
    std::is_void_v<T> or std::convertible_to<T, Value>;

// Implementation detail. A concept capturing one way a type can adhere to the
// `Instruction` concept defined below.
template <typename Signature>
concept StatelessWithoutImmediateValues =
    VoidOrConvertibleToValue<typename Signature::return_type> and
    Signature::invoke_with_argument_types([
    ]<std::convertible_to<Value>... Ts>() { return true; });

// Implementation detail. A concept capturing one way a type can adhere to the
// `Instruction` concept defined below.
template <typename Signature>
concept StatelessWithImmediateValues =
    std::is_void_v<typename Signature::return_type> and
    Signature::invoke_with_argument_types([
    ]<std::same_as<ValueStack &>, std::convertible_to<Value>... Ts>() {
      return not(std::is_reference_v<Ts> or ...);
    });

// Implementation detail. A concept capturing one way a type can adhere to the
// `Instruction` concept defined below.
template <typename Signature, typename I>
concept StatefulWithImmediateValues =
    std::is_void_v<typename Signature::return_type> and
    Signature::invoke_with_argument_types(
        []<std::same_as<ValueStack &>,
           std::same_as<typename I::JasminFunctionState &>,
           std::convertible_to<Value>... Ts>() {
          return not(std::is_reference_v<Ts> or ...);
        });

// Implementation detail. A concept capturing one way a type can adhere to the
// `Instruction` concept defined below.
template <typename Signature, typename I>
concept StatefulWithoutImmediateValues =
    VoidOrConvertibleToValue<typename Signature::return_type> and
    Signature::invoke_with_argument_types(
        []<std::same_as<typename I::JasminFunctionState &>,
           std::convertible_to<Value>... Ts>() { return true; });

// Implementation detail. A concept capturing that the `execute` static member
// function's signature is a valid signature for a stateful instruction `I`.
template <typename Signature, typename I>
concept ValidStatefulSignature = (StatefulWithImmediateValues<Signature, I> or
                                  StatefulWithoutImmediateValues<Signature, I>);

// Implementation detail. A concept capturing that the `execute` static member
// function's signature for a stateless instruction.
template <typename Signature>
concept ValidStatelessSignature = (StatelessWithImmediateValues<Signature> or
                                   StatelessWithoutImmediateValues<Signature>);

template <typename I>
concept InstructionWithoutImediateValues =
    (StatelessWithoutImmediateValues<ExtractSignature<decltype(&I::execute)>> or
     StatefulWithoutImmediateValues<ExtractSignature<decltype(&I::execute)>,
                                    I>);

// Implementation detail. A concept capturing that the `execute` static member
// function of the instruction adheres to one of the supported signatures.
template <typename I,
          typename Signature = ExtractSignature<decltype(&I::execute)>>
concept HasValidSignature = (ValidStatefulSignature<Signature, I> or
                             ValidStatelessSignature<Signature>);

template <typename T>
struct NotVoid {
  static constexpr bool value = not std::is_void_v<T>;
};

template <typename T>
concept HasFunctionState = requires {
  typename T::JasminFunctionState;
};

template <typename T>
struct GetFunctionStateImpl {
  using type = void;
};

template <HasFunctionState T>
struct GetFunctionStateImpl<T> {
  using type = typename T::JasminFunctionState;
};

template <typename T>
using GetFunctionState = typename GetFunctionStateImpl<T>::type;

// A list of all types required to represent the state of all functions in
// the instruction set `Set`.
template <typename Set>
using FunctionStateList = Filter<
    NotVoid,
    Unique<Transform<GetFunctionState, typename Set::jasmin_instructions *>>>;

}  // namespace internal

// A concept indicating which types constitute instruction sets understandable
// by Jasmin's interpreter.
template <typename T>
concept InstructionSet = std::derived_from<T, internal::InstructionSetBase>;

// A type-function accepting an instruction set and returning a type sufficient
// to hold all state required by all instructions in `Set`, or `void` if all
// instructions in `Set` are stateless.
template <InstructionSet Set>
using FunctionStateStack = std::conditional_t<
    std::is_same_v<internal::FunctionStateList<Set>, internal::type_list<>>,
    void,
    std::stack<internal::Apply<std::tuple, internal::FunctionStateList<Set>>>>;

// Forward declarations for instructions that need special treatement in
// Jasmin's interpreter and are built-in to every instruction set. Definitions
// appear below.
struct Call;
struct Jump;
struct JumpIf;
struct Return;

// Every instruction `Inst` executable as part of Jasmin's stack machine
// interpreter must publicly inherit from `StackMachineInstruction<Inst>`.
// Moreover, other than the builtin-instructions forward-declared above, they
// must also have a static member function `execute` that is not part of an
// overload set (so that `&Inst::execute` is well-formed) and this function must
// adhere to one of the following:
//
//   (a) Return void and accept a `jasmin::ValueStack&`, and then some number of
//       arguments convertible to `Value`. These arguments are intperpreted as
//       the immediate values for the instruction. The void return type
//       indicates that execution should fall through to the following
//       instruction.
//
//   (b) Return void and accept a `jasmin::ValueStack&`, a mutable reference to
//       a `typename I::JasminFunctionState` (provided this symbol is
//       well-formed), and then some number of arguments convertible to `Value`.
//       These arguments are intperpreted as the immediate values for the
//       instruction. The void return type indicates that execution should fall
//       through to the following instruction.
//
//   (c) Accept some number of arguments convertible to `Value` and return
//       either `void` or another type convertible to `Value`. The arguments are
//       to be popped from the value stack, and the returned value, if any, will
//       be pushed onto the value stack. Execution will fall through to the
//       following instruction.
//
//   (d) Accept a mutable reference to a `typename I::JasminFunctionState`
//       (provided this symbol is well-formed), and then some number of
//       arguments convertible to `Value` and return either `void` or another
//       type convertible to `Value`. The arguments are to be popped from the
//       value stack, and the returned value, if any, will be pushed onto the
//       value stack. Execution will fall through to the following instruction.
//
template <typename Inst>
struct StackMachineInstruction {
 private:
  template <InstructionSet Set>
  static void ExecuteImpl(ValueStack &value_stack, InstructionPointer &ip,
                          CallStack &call_stack, void *state_stack) {
    if constexpr (std::is_same_v<Inst, Call>) {
      auto const *f = value_stack.pop<internal::FunctionBase const *>();
      call_stack.push(f, ip);
      ip = f->entry();
      if constexpr (not std::is_void_v<FunctionStateStack<Set>>) {
        reinterpret_cast<FunctionStateStack<Set> *>(state_stack)->emplace();
      }
      JASMIN_INTERNAL_TAIL_CALL return ip->as<internal::exec_fn_type>()(
          value_stack, ip, call_stack, state_stack);

    } else if constexpr (std::is_same_v<Inst, Jump>) {
      ip += (ip + 1)->as<ptrdiff_t>();

      JASMIN_INTERNAL_TAIL_CALL return ip->as<internal::exec_fn_type>()(
          value_stack, ip, call_stack, state_stack);

    } else if constexpr (std::is_same_v<Inst, JumpIf>) {
      if (value_stack.pop<bool>()) {
        ip += (ip + 1)->as<ptrdiff_t>();
      } else {
        ip += 2;
      }

      JASMIN_INTERNAL_TAIL_CALL return ip->as<internal::exec_fn_type>()(
          value_stack, ip, call_stack, state_stack);

    } else if constexpr (std::is_same_v<Inst, Return>) {
      ip = call_stack.pop();
      if constexpr (not std::is_void_v<FunctionStateStack<Set>>) {
        reinterpret_cast<FunctionStateStack<Set> *>(state_stack)->pop();
      }
      ++ip;
      if (call_stack.empty()) [[unlikely]] {
        return;
      } else {
        JASMIN_INTERNAL_TAIL_CALL return ip->as<internal::exec_fn_type>()(
            value_stack, ip, call_stack, state_stack);
      }
    } else {
      using signature = internal::ExtractSignature<decltype(&Inst::execute)>;

      if constexpr (internal::StatelessWithImmediateValues<signature>) {
        signature::
            invoke_with_argument_types([&]<std::same_as<ValueStack &>,
                                           std::convertible_to<Value>... Ts>() {
              // Brace-initialization forces the order of evaluation to be in
              // the order the elements appear in the list.
              std::apply(Inst::execute, std::tuple<ValueStack &, Ts...>{
                                            value_stack, (++ip)->as<Ts>()...});
            });
        ++ip;
      } else if constexpr (internal::StatelessWithoutImmediateValues<
                               signature>) {
        if constexpr (std::is_void_v<typename signature::return_type>) {
          signature::invoke_with_argument_types(
              [&]<std::convertible_to<Value>... Ts>() {
                std::apply(Inst::execute, value_stack.pop_suffix<Ts...>());
              });
        } else {
          signature::invoke_with_argument_types(
              [&]<std::convertible_to<Value>... Ts>() {
                value_stack.call_on_suffix<&Inst::execute, Ts...>();
              });
        }
        ++ip;
      } else if constexpr (internal::StatefulWithImmediateValues<signature,
                                                                 Inst>) {
        signature::invoke_with_argument_types(
            [&]<std::same_as<ValueStack &>,
                std::same_as<typename Inst::JasminFunctionState &>,
                std::convertible_to<Value>... Ts>() {
              // Brace-initialization forces the order of evaluation to be in
              // the order the elements appear in the list.
              std::apply(
                  Inst::execute,
                  std::tuple<ValueStack &, typename Inst::JasminFunctionState &,
                             Ts...>{
                      value_stack,
                      std::get<typename Inst::JasminFunctionState>(
                          reinterpret_cast<FunctionStateStack<Set> *>(
                              state_stack)
                              ->top()),
                      (++ip)->as<Ts>()...});
            });
        ++ip;
      } else {
        if constexpr (std::is_void_v<typename signature::return_type>) {
          signature::invoke_with_argument_types(
              [&]<std::same_as<typename Inst::JasminFunctionState &>,
                  std::convertible_to<Value>... Ts>() {
                std::apply(
                    [&](auto... values) {
                      Inst::execute(
                          std::get<typename Inst::JasminFunctionState>(
                              reinterpret_cast<FunctionStateStack<Set> *>(
                                  state_stack)
                                  ->top()),
                          values...);
                    },
                    value_stack.pop_suffix<Ts...>());
              });
        } else {
          signature::invoke_with_argument_types(
              [&]<std::same_as<typename Inst::JasminFunctionState &>,
                  std::convertible_to<Value>... Ts>() {
                value_stack.call_on_suffix<&Inst::execute, Ts...>(
                    std::get<typename Inst::JasminFunctionState>(
                        reinterpret_cast<FunctionStateStack<Set> *>(state_stack)
                            ->top()));
              });
        }
        ++ip;
      }
    }

    JASMIN_INTERNAL_TAIL_CALL return ip->as<internal::exec_fn_type>()(
        value_stack, ip, call_stack, state_stack);
  }
};

// Built-in instructions to every instruction-set.
struct Call : StackMachineInstruction<Call> {};
struct Jump : StackMachineInstruction<Jump> {};
struct JumpIf : StackMachineInstruction<JumpIf> {};
struct Return : StackMachineInstruction<Return> {};

// The `Instruction` concept indicates that a type `I` represents an instruction
// which Jasmin is capable of executing. Instructions must either be one of the
// builtin instructions `jasmin::Call`, `jasmin::Jump`, `jasmin::JumpIf`, or
// `jasmin::Return`, or publicly inherit from
// `jasmin::StackMachineInstruction<I>` and have a static member function
// `execute` that is not part of an overload set, and that adheres to one of the
// following:
//
//   (a) Return void and accept a `jasmin::ValueStack&`, and then some number of
//       arguments convertible to `Value`. These arguments are intperpreted as
//       the immediate values for the instruction. The void return type
//       indicates that execution should fall through to the following
//       instruction.
//
//   (b) Return void and accept a `jasmin::ValueStack&`, a mutable reference to
//       a `typename I::JasminFunctionState` (provided this symbol is
//       well-formed), and then some number of arguments convertible to `Value`.
//       These arguments are intperpreted as the immediate values for the
//       instruction. The void return type indicates that execution should fall
//       through to the following instruction.
//
//   (c) Accept some number of arguments convertible to `Value` and return
//       either `void` or another type convertible to `Value`. The arguments are
//       to be popped from the value stack, and the returned value, if any, will
//       be pushed onto the value stack. Execution will fall through to the
//       following instruction.
//
//   (d) Accept a mutable reference to a `typename I::JasminFunctionState`
//       (provided this symbol is well-formed), and then some number of
//       arguments convertible to `Value` and return either `void` or another
//       type convertible to `Value`. The arguments are to be popped from the
//       value stack, and the returned value, if any, will be pushed onto the
//       value stack. Execution will fall through to the following instruction.
//
template <typename I>
concept Instruction = (internal::AnyOf<I, Call, Jump, JumpIf, Return> or
                       (std::derived_from<I, StackMachineInstruction<I>> and
                        internal::HasValidSignature<I>));

namespace internal {
template <typename I>
concept InstructionOrInstructionSet = Instruction<I> or InstructionSet<I>;

// Constructs an InstructionSet type from a list of instructions. Does no
// checking to validate that `Is` do not contain repeats.
template <Instruction... Is>
struct MakeInstructionSet final : InstructionSetBase {
  using jasmin_instructions = void(Is *...);

  // Returns the number of instructions in the instruction set.
  static constexpr size_t size() { return sizeof...(Is); }

  // Returns a `uint64_t` indicating the op-code for the given template
  // parameter instruction `I`.
  template <AnyOf<Is...> I>
  static constexpr uint64_t OpCodeFor() {
    constexpr size_t value = OpCodeForImpl<I>();
    return value;
  }

  static auto InstructionFunction(uint64_t op_code) {
    JASMIN_INTERNAL_DEBUG_ASSERT(op_code < sizeof...(Is),
                                 "Out-of-bounds op-code");
    return table[op_code];
  }

 private:
  static constexpr exec_fn_type table[sizeof...(Is)] = {
      &Is::template ExecuteImpl<MakeInstructionSet>...};

  template <AnyOf<Is...> I>
  static constexpr uint64_t OpCodeForImpl() {
    // Because the fold-expression below unconditionally adds one to `i` on its
    // first evaluation, we start `i` at its maximum value and allow it to wrap
    // around.
    uint64_t i = std::numeric_limits<uint64_t>::max();
    static_cast<void>(((++i, std::is_same_v<I, Is>) or ...));
    return i;
  }
};

// Given a list of `Instruction`s or `InstructionSet`s, `FlattenInstructionList`
// computes the list of all instructions in the list, in an InstructionSet in
// the list transitively.
template <Instruction... Processed>
constexpr auto FlattenInstructionList(type_list<Processed...>, type_list<>) {
  return type_list<Processed...>{};
}

template <Instruction... Processed, InstructionOrInstructionSet I,
          InstructionOrInstructionSet... Is>
constexpr auto FlattenInstructionList(type_list<Processed...>,
                                      type_list<I, Is...>) {
  if constexpr (AnyOf<I, Processed...>) {
    return FlattenInstructionList(type_list<Processed...>{},
                                  type_list<Is...>{});
  } else if constexpr (Instruction<I>) {
    return FlattenInstructionList(type_list<Processed..., I>{},
                                  type_list<Is...>{});
  } else {
    return FlattenInstructionList(
        type_list<Processed...>{},
        Concatenate<type_list<Is...>, typename I::jasmin_instructions *>{});
  }
}

using BuiltinInstructionList = type_list<Call, Jump, JumpIf, Return>;

}  // namespace internal

template <internal::InstructionOrInstructionSet... Is>
using MakeInstructionSet =
    internal::Apply<internal::MakeInstructionSet,
                    decltype(internal::FlattenInstructionList(
                        /*processed=*/internal::BuiltinInstructionList{},
                        /*unprocessed=*/internal::type_list<Is...>{}))>;

namespace internal {

template <Instruction I>
constexpr size_t ImmediateValueCount() {
  if constexpr (AnyOf<I, Call, Return>) {
    return 0;
  } else if constexpr (AnyOf<I, Jump, JumpIf>) {
    return 1;
  } else {
    using signature = ExtractSignature<decltype(&I::execute)>;
    if constexpr (StatelessWithoutImmediateValues<signature>) {
      return 0;
    } else if constexpr (StatelessWithImmediateValues<signature>) {
      return signature::invoke_with_argument_types(
          []<typename... Ts>() { return sizeof...(Ts) - 1; });
    } else if constexpr (StatefulWithImmediateValues<signature, I>) {
      return signature::invoke_with_argument_types(
          []<typename... Ts>() { return sizeof...(Ts) - 2; });
    } else if constexpr (StatefulWithoutImmediateValues<signature, I>) {
      return 0;
    }
  }
}

}  // namespace internal
}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_H
