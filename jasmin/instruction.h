#ifndef JASMIN_INSTRUCTION_H
#define JASMIN_INSTRUCTION_H

#include "jasmin/call_stack.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/internal/attributes.h"
#include "jasmin/internal/function_base.h"
#include "jasmin/internal/type_traits.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"

namespace jasmin {

// TODO: Remove the need for these instructions, or come to some better
// understanding about their necessity.
struct Return;
struct JumpIf;
struct Call;

template <typename Inst>
struct StackMachineInstruction {
 private:
  template <typename... Instructions>
  friend struct InstructionTable;

  template <typename InstructionTableType>
  static void execute_impl(ValueStack &value_stack, InstructionPointer &ip,
                           CallStack &call_stack) {
    if constexpr (std::is_same_v<Inst, Call>) {
      auto const *f =
          value_stack.pop<internal_function_base::FunctionBase const *>();
      call_stack.push(f, value_stack.size(), ip);
      ip = f->entry();
      JASMIN_INTERNAL_TAIL_CALL return InstructionTableType::table
          [ip->op_code()](value_stack, ip, call_stack);
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
        JASMIN_INTERNAL_TAIL_CALL return InstructionTableType::table
            [ip->op_code()](value_stack, ip, call_stack);
      }
    } else if constexpr (std::is_same_v<Inst, JumpIf>) {
      ++ip;
      if (value_stack.pop<bool>()) {
        ip = call_stack.back()->entry() + ip->value().as<ptrdiff_t>();
      } else {
        ++ip;
      }
      JASMIN_INTERNAL_TAIL_CALL return InstructionTableType::table
          [ip->op_code()](value_stack, ip, call_stack);
    } else {
      using signature =
          internal_type_traits::ExtractSignature<decltype(&Inst::execute)>;

      if constexpr (requires { Inst::execute(value_stack, ip); }) {
        static_assert(std::is_void_v<decltype(Inst::execute(value_stack, ip))>);
        Inst::execute(value_stack, ip);
      } else {
        if constexpr (std::is_void_v<typename signature::return_type>) {
          signature::invoke_with_argument_types([&]<typename... Ts>() {
            std::apply(Inst::execute, value_stack.pop_suffix<Ts...>());
          });
        } else {
          signature::invoke_with_argument_types([&]<typename... Ts>() {
            value_stack.push(
                std::apply(Inst::execute, value_stack.pop_suffix<Ts...>()));
          });
        }
        ++ip;
      }
    }

    JASMIN_INTERNAL_TAIL_CALL return InstructionTableType::table[ip->op_code()](
        value_stack, ip, call_stack);
  }
};

struct Call : StackMachineInstruction<Call> {};
struct JumpIf : StackMachineInstruction<JumpIf> {};
struct Return : StackMachineInstruction<Return> {};

}  // namespace jasmin
#endif  // JASMIN_INSTRUCTION_H
