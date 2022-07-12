#ifndef JASMIN_INTERNAL_FUNCTION_BASE_H
#define JASMIN_INTERNAL_FUNCTION_BASE_H

#include <vector>

namespace jasmin::internal_function_base {

struct FunctionBase {
  explicit FunctionBase(uint8_t parameter_count, uint8_t return_count)
      : parameter_count_(parameter_count), return_count_(return_count) {}

  constexpr uint8_t parameter_count() const { return parameter_count_; }
  constexpr uint8_t return_count() const { return return_count_; }

  constexpr InstructionPointer entry() const {
    return InstructionPointer(op_codes_.data());
  }

  std::vector<OpCodeOrValue> op_codes_;

 private:
  uint8_t parameter_count_;
  uint8_t return_count_;
};

}  // namespace jasmin::internal_function_base

#endif  // JASMIN_INTERNAL_FUNCTION_BASE_H
