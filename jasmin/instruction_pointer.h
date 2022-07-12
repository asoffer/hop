#ifndef JASMIN_INSTRUCTION_POINTETR_H
#define JASMIN_INSTRUCTION_POINTETR_H

#include <cstdint>

#include "jasmin/debug.h"
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
  constexpr static OpCodeOrValue Value(Value value) {
    return OpCodeOrValue(value);
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
                                 "OpCodeOrValue unexpectedly holds a op-code");
    return value_;
  }

  // Sets this `OpCodeOrValue to represent `v`.Generally,
  // behavior is undefined if this `OpCodeOrValue` does not represent a `Value`.
  // However, if the `JASMIN_DEBUG` macro is defined, then the behavior is
  // defined to report an error message to `stderr` and abort program execution.
  void set_value(struct Value v) {
    JASMIN_INTERNAL_DEBUG_ASSERT(not is_op_code_,
                                 "OpCodeOrValue unexpectedly holds a op-code");
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

#if defined(JASMIN_DEBUG)
  bool is_op_code_;
#endif

  union {
    uint64_t op_code_;
    struct Value value_;
  };
};

struct InstructionPointer {
  // Consturcts an `InstructionPointer` from an underlying `OpCodeOrValue` pointer `p`.
  explicit constexpr InstructionPointer(OpCodeOrValue const *p)
      : pointer_(p) {}

  // Returns the underlying `OpCodeOrValue` pointer.
  constexpr OpCodeOrValue const *operator->() const {
    return pointer_;
  }

  // Moves the instruction pointer forward one instruction, returning the
  // modified InstructionPointer.
  constexpr InstructionPointer operator++() {
    ++pointer_;
    return *this;
  }

  // Moves the instruction pointer forward one instruction, returning the
  // original InstructionPointer.
  constexpr InstructionPointer operator++(int) {
    auto copy = *this;
    ++copy;
    return copy;
  }

  // Moves the instruction pointer backward one instruction, returning the
  // modified InstructionPointer.
  constexpr InstructionPointer operator--() {
    --pointer_;
    return *this;
  }

  // Moves the instruction pointer backward one instruction, returning the
  // original InstructionPointer.
  constexpr InstructionPointer operator--(int) {
    auto copy = *this;
    --copy;
    return copy;
  }

  // Returns an `InstructionPointer` that is `n` instructions further forward than `ip`.
  friend constexpr InstructionPointer operator+(InstructionPointer ip,
                                                ptrdiff_t n) {
    return InstructionPointer(ip.pointer_ + n);
  }

  // Returns an `InstructionPointer` that is `n` instructions further forward than `ip`.
  friend constexpr InstructionPointer operator+(ptrdiff_t n,
                                                InstructionPointer ip) {
    return InstructionPointer(ip.pointer_ + n);
  }

  // Increments this `InstructionPointer` forwards by `n`.
  InstructionPointer &operator+=(ptrdiff_t n) {
    pointer_ += n;
    return *this;
  }

  // Returns an `InstructionPointer` that is `n` instructions further backward than `ip`.
  friend InstructionPointer operator-(InstructionPointer ip, ptrdiff_t n) {
    return InstructionPointer(ip.pointer_ - n);
  }

  // Increments this `InstructionPointer` backwards by `n`.
  InstructionPointer &operator-=(ptrdiff_t n) {
    pointer_ -= n;
    return *this;
  }

 private:
  OpCodeOrValue const *pointer_;
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_POINTER
