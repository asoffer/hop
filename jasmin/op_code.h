#ifndef JASMIN_OP_CODE_H
#define JASMIN_OP_CODE_H

#include <cstddef>

namespace jasmin {

// A type describing the extents of an op-code, complete with all its subsequent
// immediate values.
struct OpCodeRange {
  // Constructs an `OpCodeRange` starting at the given `offset` and extending
  // for a length of `length`.
  explicit constexpr OpCodeRange(size_t offset, size_t length)
      : offset_(offset), length_(length) {}

  // Returns the distance between op-codes, a value sufficient for describing
  // relative jumps.
  static ptrdiff_t Distance(OpCodeRange lhs, OpCodeRange rhs) {
    return static_cast<ptrdiff_t>(lhs.offset_) -
           static_cast<ptrdiff_t>(rhs.offset_);
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

// Represents information describing an op-code.
struct OpCodeMetadata {
  size_t op_code_value;
  size_t immediate_value_count;
};

}  // namespace jasmin

#endif  // JASMIN_OP_CODE_H
