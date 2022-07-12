#ifndef JASMIN_CALL_STACK_H
#define JASMIN_CALL_STACK_H

#include <cassert>
#include <tuple>
#include <vector>

#include "jasmin/internal/function_base.h"

namespace jasmin {

struct CallStack {
  void push(internal_function_base::FunctionBase const *f,
            size_t value_stack_size, InstructionPointer ip) {
    stack_.emplace_back(f, value_stack_size, ip);
  }

  constexpr bool empty() const { return stack_.empty(); }

  internal_function_base::FunctionBase const *back() const {
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
  std::vector<std::tuple<internal_function_base::FunctionBase const *, size_t,
                         InstructionPointer>>
      stack_;
};

}  // namespace jasmin

#endif  // JASMIN_CALL_STACK_H
