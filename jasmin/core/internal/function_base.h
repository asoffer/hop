#ifndef JASMIN_CORE_INTERNAL_FUNCTION_BASE_H
#define JASMIN_CORE_INTERNAL_FUNCTION_BASE_H

#include <span>
#include <vector>

#include "jasmin/core/instruction_index.h"
#include "jasmin/core/internal/frame.h"
#include "jasmin/core/internal/function_state.h"
#include "jasmin/core/value.h"
#include "nth/container/interval.h"
#include "nth/container/stack.h"
#include "nth/debug/debug.h"

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
  explicit FunctionBase(uint32_t parameter_count, uint32_t return_count,
                        void (*invoke)(nth::stack<Value> &, Value const *))
      : instructions_{invoke},
        parameter_count_(parameter_count),
        return_count_(return_count) {}

  // Returns the number of parameters this function accepts.
  constexpr uint32_t parameter_count() const { return parameter_count_; }

  // Returns the number of values this function returns.
  constexpr uint32_t return_count() const { return return_count_; }

  // Returns a pointer to the first instruction in this function.
  constexpr Value const *entry() const { return instructions_.data() + 1; }

  // Invoke the function with arguments provided via `value_stack`.
  constexpr void invoke(nth::stack<Value> &value_stack) const {
    instructions_.data()->as<void (*)(nth::stack<Value> &, Value const *)>()(
        value_stack, entry());
  }

  // Returns a span over all values representing instructions in the function.
  // Values in the span are not distinguished separately as op-codes or
  // immediate values.
  std::span<Value const> raw_instructions() const {
    return std::span<Value const>(instructions_).subspan(1);
  }
  std::span<Value> raw_instructions() {
    return std::span<Value>(instructions_).subspan(1);
  }

  // Given the index `index` into the immediate values of `range`, sets the
  // corresponding `Value` to `value`. Behavior is undefined if `range` is not a
  // valid range in `*this` function. This is typically used in conjunction with
  // `append_with_placeholders` to defer setting a value, but may be used to
  // overwrite any such `Value`.
  void set_value(nth::interval<InstructionIndex> range,
                 InstructionIndex::difference_type index, Value value) {
    NTH_REQUIRE((harden), index + 1 < range.length());
    instructions_[range.lower_bound().value() + index + 1] = value;
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
  // Returns an `nth::interval<InstructionIndex>` representing the appended
  // sequence.
  nth::interval<InstructionIndex> append(std::initializer_list<Value> range) {
    size_t size = instructions_.size();
    instructions_.insert(instructions_.end(), range.begin(), range.end());
    return nth::interval(InstructionIndex(size),
                         InstructionIndex(size + range.size()));
  }

  // Appends the sequence of `Value`s. To the instructions. The first must
  // represent an op-code and the remainder must represent immediate values.
  // Returns an `nth::interval<InstructionIndex>` representing the appended
  // sequence.
  nth::interval<InstructionIndex> append(Value fn, size_t placeholders) {
    size_t size = instructions_.size();
    instructions_.push_back(fn);
    instructions_.resize(instructions_.size() + placeholders,
                         Value::Uninitialized());
    return nth::interval(InstructionIndex(size),
                         InstructionIndex(size + placeholders + 1));
  }

 private:
  std::vector<Value> instructions_;
  uint32_t parameter_count_;
  uint32_t return_count_;
};

}  // namespace jasmin::internal

#endif  // JASMIN_CORE_INTERNAL_FUNCTION_BASE_H
