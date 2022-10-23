#ifndef JASMIN_INTERNAL_FUNCTION_BASE_H
#define JASMIN_INTERNAL_FUNCTION_BASE_H

#include <span>
#include <vector>

#include "jasmin/instruction_pointer.h"
#include "jasmin/op_code.h"

namespace jasmin::internal {

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
    return InstructionPointer(instructions_.data());
  }

  // Returns a span over all values representing instructions in the function.
  // Values in the span are not distinguished separately as `OpCode`s or
  // arguments passed to an `OpCode`.
  std::span<Value const> raw_instructions() const { return instructions_; }

  // Given the index `index` into the immediate values of `range`, sets the
  // corresponding `Value` to `value`. Behavior is undefined if `range` is not a
  // valid range in `*this` function. This is typically used in conjunction with
  // `append_with_placeholders` to defer setting a value, but may be used to
  // overwrite any such `Value`.
  void set_value(OpCodeRange range, size_t index, Value value) {
    JASMIN_INTERNAL_DEBUG_ASSERT(index + 1 < range.size(),
                                 "Index larger than range");
    instructions_[range.offset() + index + 1] = value;
  }

  // Reserves space for up to `capacity` op-codes or immediate values before
  // reallocation would become necessary.
  void reserve(size_t capacity) { instructions_.reserve(capacity); }

  // Appends a value directly without regards to whether it is an op-code or
  // immediate value.
  void raw_append(Value v) { instructions_.push_back(v); }


 protected:
  // Appends the sequence of `Value`s. To the instructions. The first must
  // represent an op-code and the remainder must represent immediate values.
  // Returns an `OpCodeRange` representing the appended sequence.
  OpCodeRange append(std::initializer_list<Value> range) {
    size_t size = instructions_.size();
    instructions_.insert(instructions_.end(), range.begin(), range.end());
    return OpCodeRange(size, range.size());
  }

  // Appends the sequence of `Value`s. To the instructions. The first must
  // represent an op-code and the remainder must represent immediate values.
  // Returns an `OpCodeRange` representing the appended sequence.
  OpCodeRange append(Value fn, size_t placeholders) {
    size_t size = instructions_.size();
    instructions_.push_back(fn);
    instructions_.resize(instructions_.size() + placeholders,
                         Value::Uninitialized());
    return OpCodeRange(size, placeholders + 1);
  }

 private:
  std::vector<Value> instructions_;
  uint8_t parameter_count_;
  uint8_t return_count_;
};

}  // namespace jasmin::internal

#endif  // JASMIN_internal_H
