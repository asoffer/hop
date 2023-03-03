#ifndef JASMIN_TESTING_H
#define JASMIN_TESTING_H

#include "jasmin/instruction.h"

namespace jasmin {
namespace internal {

template <typename Inst, bool Imm, bool E, bool F>
concept ValidInstruction =
    Instruction<Inst> and
    (HasValueStack<internal::ExtractSignature<decltype(&Inst::execute)>> ==
     Imm) and
    (HasExecutionState<Inst> == E) and (HasFunctionState<Inst> == F);

}  // namespace internal

template <internal::ValidInstruction<false, false, false> Inst>
void ExecuteInstruction(ValueStack &value_stack) {
  using Signature = internal::ExtractSignature<decltype(&Inst::execute)>;
  if constexpr (std::is_void_v<typename Signature::return_type>) {
    Signature::invoke_with_argument_types([&]<
                                          std::convertible_to<Value>... Ts>() {
      std::apply(Inst::execute, value_stack.pop_suffix<Ts...>());
    });
  } else {
    Signature::invoke_with_argument_types([&]<
                                          std::convertible_to<Value>... Ts>() {
      value_stack.call_on_suffix<&Inst::execute, Ts...>();
    });
  }
}

template <internal::ValidInstruction<false, false, true> Inst>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::function_state &fn_state) {
  using Signature = internal::ExtractSignature<decltype(&Inst::execute)>;
  Signature::invoke_with_argument_types(
      [&]<std::same_as<typename Inst::function_state &>,
          std::convertible_to<Value>... Ts>() {
        std::apply([&](auto... values) { Inst::execute(fn_state, values...); },
                   value_stack.pop_suffix<Ts...>());
      });
}

template <internal::ValidInstruction<false, true, false> Inst>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::execution_state &exec_state) {
  using Signature = internal::ExtractSignature<decltype(&Inst::execute)>;
  Signature::invoke_with_argument_types(
      [&]<std::same_as<typename Inst::execution_state &>,
          std::convertible_to<Value>... Ts>() {
        std::apply(
            [&](auto... values) { Inst::execute(exec_state, values...); },
            value_stack.pop_suffix<Ts...>());
      });
}

template <internal::ValidInstruction<false, true, true> Inst>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::execution_state &exec_state,
                        typename Inst::function_state &fn_state) {
  using Signature = internal::ExtractSignature<decltype(&Inst::execute)>;
  Signature::invoke_with_argument_types(
      [&]<std::same_as<typename Inst::execution_state &>,
          std::same_as<typename Inst::function_state &>,
          std::convertible_to<Value>... Ts>() {
        std::apply(
            [&](auto... values) {
              Inst::execute(exec_state, fn_state, values...);
            },
            value_stack.pop_suffix<Ts...>());
      });
}

template <internal::ValidInstruction<true, false, false> Inst,
          typename... ImmediateArguments>
void ExecuteInstruction(ValueStack &value_stack,
                        ImmediateArguments... immediate_arguments) {
  Inst::execute(value_stack, immediate_arguments...);
}

template <internal::ValidInstruction<true, false, true> Inst,
          typename... ImmediateArguments>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::function_state &fn_state,
                        ImmediateArguments... immediate_arguments) {
  Inst::execute(value_stack, fn_state, immediate_arguments...);
}

template <internal::ValidInstruction<true, true, false> Inst,
          typename... ImmediateArguments>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::execution_state &exec_state,
                        ImmediateArguments... immediate_arguments) {
  Inst::execute(value_stack, exec_state, immediate_arguments...);
}

template <internal::ValidInstruction<true, true, true> Inst,
          typename... ImmediateArguments>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::execution_state &exec_state,
                        typename Inst::function_state &fn_state,
                        ImmediateArguments... immediate_arguments) {
  Inst::execute(value_stack, exec_state, fn_state, immediate_arguments...);
}

}  // namespace jasmin

#endif  // JASMIN_TESTING_H
