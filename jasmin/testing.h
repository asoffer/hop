#ifndef JASMIN_TESTING_H
#define JASMIN_TESTING_H

#include "jasmin/execute.h"
#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "nth/container/stack.h"

namespace jasmin {

template <InstructionType Inst>
requires(not internal::HasFunctionState<Inst> and
         not internal::BuiltinInstruction<Inst>())  //
    void ExecuteInstruction(nth::stack<Value> &value_stack,
                            auto... immediates) {
  Function<MakeInstructionSet<Inst>> f(0, 0);
  f.template append<Inst>(immediates...);
  f.template append<Return>();
  Execute(f, value_stack);
}

template <InstructionType Inst>
requires(internal::HasFunctionState<Inst> and
         not internal::BuiltinInstruction<Inst>())  //
    void ExecuteInstruction(nth::stack<Value> &value_stack,
                            typename Inst::function_state &state,
                            auto... immediates) {
  struct WriteFunctionState : Instruction<WriteFunctionState> {
    using function_state = typename Inst::function_state;
    static void execute(function_state &state, std::span<Value, 0>,
                        function_state *in) {
      state = std::move(*in);
    }
  };
  struct ExtractFunctionState : Instruction<ExtractFunctionState> {
    using function_state = typename Inst::function_state;
    static void execute(function_state &state, std::span<Value, 0>,
                        function_state *out) {
      *out = std::move(state);
    }
  };
  Function<MakeInstructionSet<Inst, WriteFunctionState, ExtractFunctionState>>
      f(0, 0);
  f.template append<WriteFunctionState>(&state);
  f.template append<Inst>(immediates...);
  f.template append<ExtractFunctionState>(&state);
  f.template append<Return>();
  Execute(f, value_stack);
}

}  // namespace jasmin

#endif  // JASMIN_TESTING_H
