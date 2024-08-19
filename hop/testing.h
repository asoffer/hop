#ifndef JASMIN_TESTING_H
#define JASMIN_TESTING_H

#include "hop/core/function.h"
#include "hop/core/input.h"
#include "hop/core/instruction.h"
#include "hop/core/output.h"
#include "nth/container/stack.h"

namespace hop {

template <InstructionType Inst>
requires(not internal::HasFunctionState<Inst> and
         not internal::BuiltinInstruction<Inst>())  //
    void ExecuteInstruction(nth::stack<Value> &value_stack,
                            auto... immediates) {
  Function<MakeInstructionSet<Inst>> f(0, 0);
  f.template append<Inst>(immediates...);
  f.template append<Return>();
  f.invoke(value_stack);
}

template <InstructionType Inst>
requires(internal::HasFunctionState<Inst> and
         not internal::BuiltinInstruction<Inst>())  //
    void ExecuteInstruction(nth::stack<Value> &value_stack,
                            typename Inst::function_state &state,
                            auto... immediates) {
  struct WriteFunctionState : Instruction<WriteFunctionState> {
    using function_state = typename Inst::function_state;
    static void execute(function_state &state, Input<>, Output<>,
                        function_state *in) {
      state = std::move(*in);
    }
  };
  struct ExtractFunctionState : Instruction<ExtractFunctionState> {
    using function_state = typename Inst::function_state;
    static void execute(function_state &state, Input<>, Output<>,
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
  f.invoke(value_stack);
}

}  // namespace hop

#endif  // JASMIN_TESTING_H
