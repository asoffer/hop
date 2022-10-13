#ifndef JASMIN_TESTING_H
#define JASMIN_TESTING_H

#include "jasmin/instruction.h"

namespace jasmin {

template <Instruction Inst, typename... ImmediateArguments,
          internal::StatelessWithImmediateValues Signature =
              internal::ExtractSignature<decltype(&Inst::execute)>>
void ExecuteInstruction(ValueStack &value_stack,
                        ImmediateArguments... immediate_arguments) {
  Inst::execute(value_stack, immediate_arguments...);
}

template <Instruction Inst, int &...,
          internal::StatelessWithoutImmediateValues Signature =
              internal::ExtractSignature<decltype(&Inst::execute)>>
void ExecuteInstruction(ValueStack &value_stack) {
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

template <Instruction Inst, typename... ImmediateArguments,
          internal::StatefulWithImmediateValues<Inst> Signature =
              internal::ExtractSignature<decltype(&Inst::execute)>>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::JasminFunctionState &state,
                        ImmediateArguments... immediate_values) {
  Inst::execute(value_stack, state, immediate_values...);
}

template <Instruction Inst, int &...,
          internal::StatefulWithoutImmediateValues<Inst> Signature =
              internal::ExtractSignature<decltype(&Inst::execute)>>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::JasminFunctionState &state) {
  Signature::invoke_with_argument_types(
      [&]<std::same_as<typename Inst::JasminFunctionState &>,
          std::convertible_to<Value>... Ts>() {
        std::apply([&](auto... values) { Inst::execute(state, values...); },
                   value_stack.pop_suffix<Ts...>());
      });
}

}  // namespace jasmin

#endif  // JASMIN_TESTING_H
