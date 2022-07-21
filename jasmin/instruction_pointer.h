#ifndef JASMIN_INSTRUCTION_POINTETR_H
#define JASMIN_INSTRUCTION_POINTETR_H

#include <cstddef>
#include <cstdint>

#include "jasmin/internal/debug.h"
#include "jasmin/value.h"

namespace jasmin {

struct InstructionPointer {
  // Consturcts an `InstructionPointer` from an underlying `Value`
  // pointer `p`.
  explicit constexpr InstructionPointer(Value const *p) : pointer_(p) {}

  // Compares two instruction pointers for equality. Behavior is undefined if
  // the two instructionpointers do not point into the same function.
  constexpr bool operator==(InstructionPointer const &) const = default;
  constexpr bool operator!=(InstructionPointer const &) const = default;

  // Returns the underlying `Value` pointer.
  constexpr Value const *operator->() const { return pointer_; }

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
    ++*this;
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
    --*this;
    return copy;
  }

  // Returns an `InstructionPointer` that is `n` instructions further forward
  // than `ip`.
  friend constexpr InstructionPointer operator+(InstructionPointer ip,
                                                ptrdiff_t n) {
    return InstructionPointer(ip.pointer_ + n);
  }

  // Returns an `InstructionPointer` that is `n` instructions further forward
  // than `ip`.
  friend constexpr InstructionPointer operator+(ptrdiff_t n,
                                                InstructionPointer ip) {
    return InstructionPointer(ip.pointer_ + n);
  }

  // Increments this `InstructionPointer` forwards by `n`.
  InstructionPointer &operator+=(ptrdiff_t n) {
    pointer_ += n;
    return *this;
  }

  // Returns an `InstructionPointer` that is `n` instructions further backward
  // than `ip`.
  friend InstructionPointer operator-(InstructionPointer ip, ptrdiff_t n) {
    return InstructionPointer(ip.pointer_ - n);
  }

  // Increments this `InstructionPointer` backwards by `n`.
  InstructionPointer &operator-=(ptrdiff_t n) {
    pointer_ -= n;
    return *this;
  }

 private:
  Value const *pointer_;
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_POINTER
