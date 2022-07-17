#ifndef JASMIN_INSTRUCTION_H
#define JASMIN_INSTRUCTION_H

#include <concepts>

#include "jasmin/call_stack.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/internal/attributes.h"
#include "jasmin/internal/function_base.h"
#include "jasmin/internal/type_list.h"
#include "jasmin/internal/type_traits.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"

namespace jasmin {
namespace internal_instruction {

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
concept SignatureSatisfiesRequirementsNoImmediateValues =
    VoidOrConvertibleToValue<typename Signature::return_type> and
    Signature::invoke_with_argument_types([
    ]<std::convertible_to<Value>... Ts>() { return true; });

// Implementation detail. A concept capturing one way a type can adhere to the
// `Instruction` concept defined below.
template <typename Signature>
concept SignatureSatisfiesRequirementsWithImmediateValues =
    std::is_void_v<typename Signature::return_type> and
    Signature::invoke_with_argument_types([
    ]<std::same_as<ValueStack &>, std::convertible_to<Value>... Ts>() {
      return not(std::is_reference_v<Ts> or ...);
    });

// Implementation detail. A concept capturing that the signature for an
// `execute` static member function satisfies the requirements for the
// Instruction concept.
template <typename Signature>
concept SignatureSatisfiesRequirements =
    (SignatureSatisfiesRequirementsNoImmediateValues<Signature> or
     SignatureSatisfiesRequirementsWithImmediateValues<Signature>);

}  // namespace internal_instruction

// A concept indicating which types constitute instruction sets understandable
// by Jasmin's interpreter.
template <typename T>
concept InstructionSet =
    std::derived_from<T, internal_instruction::InstructionSetBase>;

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
// either:
//
//   (a) Return void and accept a `jasmin::ValueStack&`, and then some number of
//       arguments convertible to `Value`. These arguments are intperpreted as
//       the immediate values for the instruction. The void return type
//       indicates that execution should fall through to the following
//       instruction.
//
//   (b) Accept some number of arguments convertible to `Value` and return
//       either `void` or another type convertible to `Value`. The arguments are
//       to be popped from the value stack, and the returned value, if any, will
//       be pushed onto the value stack. Execution will fall through to the
//       following instruction.
//
template <typename Inst>
struct StackMachineInstruction {
 private:
  template <InstructionSet Set>
  static void ExecuteImpl(ValueStack &value_stack, InstructionPointer &ip,
                          CallStack &call_stack) {
    if constexpr (std::is_same_v<Inst, Call>) {
      auto const *f =
          value_stack.pop<internal_function_base::FunctionBase const *>();
      call_stack.push(f, value_stack.size(), ip);
      ip = f->entry();
      JASMIN_INTERNAL_TAIL_CALL return Set::InstructionFunction(ip->op_code())(
          value_stack, ip, call_stack);

    } else if constexpr (std::is_same_v<Inst, Jump>) {
      ++ip;
      ip += ip->value().as<ptrdiff_t>();

      JASMIN_INTERNAL_TAIL_CALL return Set::InstructionFunction(ip->op_code())(
          value_stack, ip, call_stack);

    } else if constexpr (std::is_same_v<Inst, JumpIf>) {
      ++ip;
      if (value_stack.pop<bool>()) {
        ip += ip->value().as<ptrdiff_t>();
      } else {
        ++ip;
      }

      JASMIN_INTERNAL_TAIL_CALL return Set::InstructionFunction(ip->op_code())(
          value_stack, ip, call_stack);

    } else if constexpr (std::is_same_v<Inst, Return>) {
      // When a call instruction is executed, all the arguments are pushed onto
      // the stack followed by the to-be-called function.
      auto [start, end] = call_stack.erasable_range(value_stack.size());
      value_stack.erase(start, end);
      ip = call_stack.pop();
      ++ip;
      if (call_stack.empty()) {
        return;
      } else {
        JASMIN_INTERNAL_TAIL_CALL return Set::InstructionFunction(
            ip->op_code())(value_stack, ip, call_stack);
      }
    } else {
      using signature = internal::ExtractSignature<decltype(&Inst::execute)>;

      if constexpr (internal_instruction::
                        SignatureSatisfiesRequirementsWithImmediateValues<
                            signature>) {
        signature::
            invoke_with_argument_types([&]<std::same_as<ValueStack &>,
                                           std::convertible_to<Value>... Ts>() {
              // Brace-initialization forces the order of evaluation to be in
              // the order the elements appear in the list.
              std::apply(Inst::execute,
                         std::tuple<ValueStack &, Ts...>{
                             value_stack, (++ip)->value().as<Ts>()...});
            });
        ++ip;
      } else {
        if constexpr (std::is_void_v<typename signature::return_type>) {
          signature::invoke_with_argument_types(
              [&]<std::convertible_to<Value>... Ts>() {
                std::apply(Inst::execute, value_stack.pop_suffix<Ts...>());
              });
        } else {
          signature::invoke_with_argument_types(
              [&]<std::convertible_to<Value>... Ts>() {
                value_stack.push(
                    std::apply(Inst::execute, value_stack.pop_suffix<Ts...>()));
              });
        }
        ++ip;
      }
    }

    JASMIN_INTERNAL_TAIL_CALL return Set::InstructionFunction(ip->op_code())(
        value_stack, ip, call_stack);
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
//   (b) Accept some number of arguments convertible to `Value` and return
//       either `void` or another type convertible to `Value`. The arguments are
//       to be popped from the value stack, and the returned value, if any, will
//       be pushed onto the value stack. Execution will fall through to the
//       following instruction.
//
template <typename I>
concept Instruction =
    (internal::AnyOf<I, Call, Jump, JumpIf, Return> or
     (std::derived_from<I, StackMachineInstruction<I>> and
      internal_instruction::SignatureSatisfiesRequirements<
          internal::ExtractSignature<decltype(&I::execute)>>));

namespace internal_instruction {
template <typename I>
concept InstructionOrInstructionSet = Instruction<I> or InstructionSet<I>;

// Constructs an InstructionSet type from a list of instructions. Does no
// checking to validate that `Is` do not contain repeats.
template <Instruction... Is>
struct MakeInstructionSet final : internal_instruction::InstructionSetBase {
  using jasmin_instructions = void(Is *...);

  // Returns the number of instructions in the instruction set.
  static constexpr size_t size() { return sizeof...(Is); }

  // Returns a `uint64_t` indicating the op-code for the given template
  // parameter instruction `I`.
  template <internal::AnyOf<Is...> I>
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
  static constexpr void (*table[sizeof...(Is)])(ValueStack &,
                                                InstructionPointer &,
                                                CallStack &) = {
      &Is::template ExecuteImpl<MakeInstructionSet>...};

  template <internal::AnyOf<Is...> I>
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
constexpr auto FlattenInstructionList(internal::type_list<Processed...>,
                                      internal::type_list<>) {
  return internal::type_list<Processed...>{};
}

template <Instruction... Processed, InstructionOrInstructionSet I,
          InstructionOrInstructionSet... Is>
constexpr auto FlattenInstructionList(internal::type_list<Processed...>,
                                      internal::type_list<I, Is...>) {
  if constexpr (internal::AnyOf<I, Processed...>) {
    return FlattenInstructionList(internal::type_list<Processed...>{},
                                  internal::type_list<Is...>{});
  } else if constexpr (Instruction<I>) {
    return FlattenInstructionList(internal::type_list<Processed..., I>{},
                                  internal::type_list<Is...>{});
  } else {
    return FlattenInstructionList(
        internal::type_list<Processed...>{},
        internal::Concatenate<internal::type_list<Is...>,
                              typename I::jasmin_instructions *>{});
  }
}

using BuiltinInstructionList = internal::type_list<Call, Jump, JumpIf, Return>;

}  // namespace internal_instruction

template <internal_instruction::InstructionOrInstructionSet... Is>
using MakeInstructionSet = internal::Apply<
    internal_instruction::MakeInstructionSet,
    decltype(internal_instruction::FlattenInstructionList(
        /*processed=*/internal_instruction::BuiltinInstructionList{},
        /*unprocessed=*/internal::type_list<Is...>{}))>;

namespace internal_instruction {

template <typename T, typename...>
struct First {
  using type = T;
};

template <Instruction I>
constexpr size_t ImmediateValueCount() {
  if constexpr (internal::AnyOf<I, Call, Return>) {
    return 0;
  } else if constexpr (internal::AnyOf<I, Jump, JumpIf>) {
    return 1;
  } else {
    return internal::ExtractSignature<decltype(&I::execute)>::
        invoke_with_argument_types([]<typename... Ts>() {
          if constexpr (sizeof...(Ts) == 0) {
            return 0;
          } else {
            return std::is_same_v<typename First<Ts...>::type, ValueStack &>
                       ? (sizeof...(Ts) - 1)
                       : 0;
          }
        });
  }
}

}  // namespace internal_instruction
}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_H
