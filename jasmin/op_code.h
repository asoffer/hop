#ifndef JASMIN_OP_CODE_H
#define JASMIN_OP_CODE_H

#include "jasmin/internal/debug.h"
#include "jasmin/value.h"

namespace jasmin {

// An instruction is a sequence of op-codes or `Value`s. Each instruction should
// document if any `Value`s may follow it, how many and how they are to be
// interpreted.
struct OpCodeOrValue {
  // Returns an `OpCodeOrValue` representing an op-code of value `op-code`.
  constexpr static OpCodeOrValue OpCode(uint64_t op_code) {
    return OpCodeOrValue(op_code);
  }

  // Returns an `OpCodeOrValue` representing a `Value`, namely `value`.
  constexpr static OpCodeOrValue Value(struct Value value) {
    return OpCodeOrValue(value);
  }

  // Returns an `OpCodeOrValue` representing an uninitialized `Value`. This
  // value may be overwritten via a call to `set_value()`.
  constexpr static OpCodeOrValue UninitializedValue() {
    return OpCodeOrValue();
  }

  // Returns the op-code represented by this `OpCodeOrValue`. Generally,
  // behavior is undefined if this `OpCodeOrValue` represnts a `Value`. However,
  // if the `JASMIN_DEBUG` macro is defined, then the behavior is defined to
  // report an error message to `stderr` and abort program execution.
  uint64_t op_code() const {
    JASMIN_INTERNAL_DEBUG_ASSERT(is_op_code_,
                                 "OpCodeOrValue unexpectedly holds a Value");
    return op_code_;
  }

  // Returns the `Value` represented by this `OpCodeOrValue`. Generally,
  // behavior is undefined if this `OpCodeOrValue` does not represent a `Value`.
  // However, if the `JASMIN_DEBUG` macro is defined, then the behavior is
  // defined to report an error message to `stderr` and abort program execution.
  struct Value value() const {
    JASMIN_INTERNAL_DEBUG_ASSERT(not is_op_code_,
                                 "OpCodeOrValue unexpectedly holds an op-code");
    return value_;
  }

  // Sets this `OpCodeOrValue to represent `v`.Generally,
  // behavior is undefined if this `OpCodeOrValue` does not represent a `Value`.
  // However, if the `JASMIN_DEBUG` macro is defined, then the behavior is
  // defined to report an error message to `stderr` and abort program execution.
  void set_value(struct Value v) {
    JASMIN_INTERNAL_DEBUG_ASSERT(not is_op_code_,
                                 "OpCodeOrValue unexpectedly holds an op-code");
    value_ = v;
  }

 private:
  // Constructs an `OpCodeOrValue` representing an op-code.
  explicit constexpr OpCodeOrValue(uint64_t op_code)
      :
#if defined(JASMIN_DEBUG)
        is_op_code_(true),
#endif
        op_code_(op_code) {
  }

  // Constructs an `OpCodeOrValue` representing a `Value`.
  explicit constexpr OpCodeOrValue(struct Value v)
      :
#if defined(JASMIN_DEBUG)
        is_op_code_(false),
#endif
        value_(v) {
  }

  // Constructs an `OpCodeOrValue` representing an uninitialized `Value`.
  explicit constexpr OpCodeOrValue()
#if defined(JASMIN_DEBUG)
      : is_op_code_(false)
#endif
  {
  }

#if defined(JASMIN_DEBUG)
  bool is_op_code_;
#endif

  union {
    uint64_t op_code_;
    struct Value value_;
  };
};

// A type describing the extents of an op-code, complete with all its subsequent
// immediate values.
struct OpCodeRange {
  // Constructs an `OpCodeRange` starting at the given `offset` and extending
  // for a length of `length`.
  explicit constexpr OpCodeRange(size_t offset, size_t length) : offset_(offset), length_(length) {}

  // Returns the distance between op-codes, a value sufficient for describing
  // relative jumps.
  friend ptrdiff_t operator-(OpCodeRange lhs, OpCodeRange rhs) {
    return static_cast<ptrdiff_t>(lhs.offset_) - static_cast<ptrdiff_t>(rhs.offset_);
  }

  // Returns the starting offset of the `OpCodeRange`
  constexpr size_t offset() const { return offset_; }

  // Returns the length of the `OpCodeRange`
  constexpr size_t size() const { return length_; }

 private:
  OpCodeRange() = default;

  size_t offset_;
  size_t length_;
};

} // namespace jasmin

#endif  // JASMIN_OP_CODE_H
