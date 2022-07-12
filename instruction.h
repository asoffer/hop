#ifndef JASMIN_INSTRUCTION_H
#define JASMIN_INSTRUCTION_H

#include <cassert>
#include <tuple>
#include <utility>
#include <vector>

#include "jasmin/internal/attributes.h"
#include "jasmin/internal/type_traits.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"

namespace jasmin {

struct InstructionPointer;

namespace internal_instruction {

union OpCodeOrValue {
  uint64_t op_code;
  Value value;
};

InstructionPointer ConstructInstructionPointer(OpCodeOrValue const *p);

}  // namespace internal_instruction

struct InstructionPointer {
  constexpr uint64_t op_code() const { return pointer_->op_code; }
  constexpr Value value() const { return pointer_->value; }

  constexpr InstructionPointer operator++() {
    ++pointer_;
    return *this;
  }

  friend InstructionPointer operator+(InstructionPointer ip, ptrdiff_t n) {
    return InstructionPointer(ip.pointer_ + n);
  }

  friend InstructionPointer operator+(ptrdiff_t n, InstructionPointer ip) {
    return InstructionPointer(ip.pointer_ + n);
  }

 private:
  friend InstructionPointer internal_instruction::ConstructInstructionPointer(
      internal_instruction::OpCodeOrValue const *p);

  explicit constexpr InstructionPointer(
      internal_instruction::OpCodeOrValue const *p)
      : pointer_(p) {}

  internal_instruction::OpCodeOrValue const *pointer_;
};

namespace internal_instruction {

inline InstructionPointer ConstructInstructionPointer(OpCodeOrValue const *p) {
  return InstructionPointer(p);
}
}  // namespace internal_instruction

namespace internal_instruction {
struct FunctionBase {
  explicit FunctionBase(uint8_t parameter_count, uint8_t return_count)
      : parameter_count_(parameter_count), return_count_(return_count) {}

  constexpr uint8_t parameter_count() const { return parameter_count_; }
  constexpr uint8_t return_count() const { return return_count_; }

  constexpr InstructionPointer entry() const {
    return ConstructInstructionPointer(op_codes_.data());
  }

  std::vector<internal_instruction::OpCodeOrValue> op_codes_;

 private:
  uint8_t parameter_count_;
  uint8_t return_count_;
};

}  // namespace internal_instruction

struct Return;
struct JumpIf;
struct Call;

template <typename InstructionTable>
struct Function : internal_instruction::FunctionBase {
  explicit constexpr Function(uint8_t parameter_count, uint8_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  template <typename Instruction, typename... Vs>
  constexpr void append(Vs... vs) requires((std::is_convertible_v<Vs, Value> and
                                            ...)) {
    op_codes_.push_back(
        {.op_code = InstructionTable::template OpCodeFor<Instruction>()});
    (op_codes_.push_back({.value = vs}), ...);
  }

  size_t append_conditional_jump() {
    append<JumpIf>(ptrdiff_t{0});
    size_t result = op_codes_.size() - 1;
    return result;
  }

  void set_jump_target(size_t index) {
    assert(op_codes_.size() > index);
    op_codes_[index].value =
        Value(static_cast<ptrdiff_t>(op_codes_.size() - 1));
  }
};

struct CallStack {
  void push(internal_instruction::FunctionBase const *f,
            size_t value_stack_size, InstructionPointer ip) {
    stack_.emplace_back(f, value_stack_size, ip);
  }

  constexpr bool empty() const { return stack_.empty(); }

  internal_instruction::FunctionBase const *back() const {
    assert(stack_.size() != 0);
    return std::get<0>(stack_.back());
  }

  InstructionPointer pop() {
    InstructionPointer ip = std::get<2>(stack_.back());
    stack_.pop_back();
    return ip;
  }

  struct ErasableRange {
    size_t start_index;
    size_t end_index;
  };

  ErasableRange erasable_range(size_t current_value_stack_size) const {
    assert(stack_.size() != 0);
    auto [f, prev_size, ip] = stack_.back();
    assert(prev_size >= f->parameter_count());
    size_t prev_frame_end = prev_size - f->parameter_count();
    assert(prev_frame_end <= current_value_stack_size);
    return {.start_index = prev_frame_end,
            .end_index   = current_value_stack_size - f->return_count()};
  }

 private:
  std::vector<std::tuple<internal_instruction::FunctionBase const *, size_t,
                        InstructionPointer>>
      stack_;
};

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
          value_stack.pop<internal_instruction::FunctionBase const *>();
      call_stack.push(f, value_stack.size(), ip);
      ip = f->entry();
      JASMIN_INTERNAL_TAIL_CALL return InstructionTableType::table
          [ip.op_code()](value_stack, ip, call_stack);
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
            [ip.op_code()](value_stack, ip, call_stack);
      }
    } else if constexpr (std::is_same_v<Inst, JumpIf>) {
      ++ip;
      if (value_stack.pop<bool>()) {
        ip = call_stack.back()->entry() + ip.value().as<ptrdiff_t>();
      } else {
        ++ip;
      }
      JASMIN_INTERNAL_TAIL_CALL return InstructionTableType::table
          [ip.op_code()](value_stack, ip, call_stack);
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

    JASMIN_INTERNAL_TAIL_CALL return InstructionTableType::table[ip.op_code()](
        value_stack, ip, call_stack);
  }
};

struct Call : StackMachineInstruction<Call> {};
struct JumpIf : StackMachineInstruction<JumpIf> {};
struct Return : StackMachineInstruction<Return> {};

template <typename... Instructions>
struct InstructionTable {
  using function_type = Function<InstructionTable>;

  template <typename Instruction>
  static constexpr uint64_t OpCodeFor() requires(
      (std::is_same_v<Instruction, Instructions> or ...)) {
    uint64_t i = 0;
    static_cast<void>(
        ((std::is_same_v<Instruction, Instructions> ? true : (++i, false)) or
         ...));
    return i;
  }

  static constexpr void (*table[sizeof...(Instructions)])(ValueStack &,
                                                          InstructionPointer &,
                                                          CallStack &) = {
      &Instructions::template execute_impl<InstructionTable>...};
};

template <typename InstructionTableType>
void Execute(Function<InstructionTableType> const &f, ValueStack &value_stack) {
  CallStack call_stack;
  InstructionPointer ip = f.entry();
  call_stack.push(&f, value_stack.size(), ip);
  return InstructionTableType::table[ip.op_code()](value_stack, ip, call_stack);
}

template <typename InstructionTableType>
void Execute(Function<InstructionTableType> const &f,
             std::initializer_list<Value> arguments,
             SmallTrivialValue auto &...return_values) {
  ValueStack value_stack(arguments);
  int dummy;
  Execute(f, value_stack);
  (dummy = ... = (return_values =
                      value_stack.pop<std::decay_t<decltype(return_values)>>(),
                  0));
}

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_H
