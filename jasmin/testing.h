#ifndef JASMIN_TESTING_H
#define JASMIN_TESTING_H

#include "jasmin/instruction.h"

namespace jasmin {
namespace internal {

template <typename Inst, bool Imm, bool E, bool F>
concept ValidInstruction = Instruction<Inst> and(HasValueStack<Inst> == Imm) and
                           (HasExecutionState<Inst> == E) and
                           (HasFunctionState<Inst> == F);

}  // namespace internal

template <internal::ValidInstruction<false, false, false> Inst>
void ExecuteInstruction(ValueStack &value_stack) {
  constexpr auto signature = nth::type<decltype(Inst::execute)>;
  signature.parameters().reduce([&](auto... ts) {
    if constexpr (signature.return_type() == nth::type<void>) {
      std::apply(Inst::execute, value_stack.pop_suffix<nth::type_t<ts>...>());
    } else {
      value_stack.call_on_suffix<&Inst::execute, nth::type_t<ts>...>();
    }
  });
}

template <internal::ValidInstruction<false, false, true> Inst>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::function_state &fn_state) {
  constexpr auto signature = nth::type<decltype(Inst::execute)>;
  signature.parameters().template drop<1>().reduce([&](auto... ts) {
    std::apply([&](auto... values) { Inst::execute(fn_state, values...); },
               value_stack.pop_suffix<nth::type_t<ts>...>());
  });
}

template <internal::ValidInstruction<false, true, false> Inst>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::execution_state &exec_state) {
  constexpr auto signature = nth::type<decltype(Inst::execute)>;
  signature.parameters().template drop<1>().reduce([&](auto... ts) {
    std::apply([&](auto... values) { Inst::execute(exec_state, values...); },
               value_stack.pop_suffix<nth::type_t<ts>...>());
  });
}

template <internal::ValidInstruction<false, true, true> Inst>
void ExecuteInstruction(ValueStack &value_stack,
                        typename Inst::execution_state &exec_state,
                        typename Inst::function_state &fn_state) {
  constexpr auto signature = nth::type<decltype(Inst::execute)>;
  signature.parameters().template drop<2>().reduce([&](auto... ts) {
    std::apply(
        [&](auto... values) { Inst::execute(exec_state, fn_state, values...); },
        value_stack.pop_suffix<nth::type_t<ts>...>());
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
