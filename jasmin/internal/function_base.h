#ifndef JASMIN_INTERNAL_FUNCTION_BASE_H
#define JASMIN_INTERNAL_FUNCTION_BASE_H

#include <vector>

#include "jasmin/instruction_pointer.h"

namespace jasmin::internal_function_base {

// `FunctionBase` is the base class for any function-type defined via Jasmin's
// infrastructure. Op-codes are only meaningful in the presence of an
// instruction table (they indicate which instruction in the table is intended),
// but much of the infrastructure for handling functions is indpendent of the
// semantics of op-codes. Thus, such member functions that are indpendent of
// op-code semantics are defined here.
struct FunctionBase {
  // Constructs a `FunctionBase` representing a function that accepts
  // `parameter_count` parameters and returns `return_count` values.
  explicit FunctionBase(uint8_t parameter_count, uint8_t return_count)
      : parameter_count_(parameter_count), return_count_(return_count) {}

  // Returns the number of parameters this function accepts.
  constexpr uint8_t parameter_count() const { return parameter_count_; }

  // Returns the number of values this function returns.
  constexpr uint8_t return_count() const { return return_count_; }

  // Returns an `InstructionPointer` pointing to the first intsruction in this
  // function.
  constexpr InstructionPointer entry() const {
    return InstructionPointer(op_codes_.data());
  }

 protected:
  std::vector<OpCodeOrValue> op_codes_;

 private:
  uint8_t parameter_count_;
  uint8_t return_count_;
};

}  // namespace jasmin::internal_function_base

#endif  // JASMIN_INTERNAL_FUNCTION_BASE_H
