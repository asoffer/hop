#ifndef JASMIN_INSTRUCTION_INDEX_H
#define JASMIN_INSTRUCTION_INDEX_H

#include <cstddef>

namespace jasmin {

struct InstructionIndex {
  using value_type      = ptrdiff_t;
  using difference_type = ptrdiff_t;

  constexpr explicit InstructionIndex(value_type n = 0) : index_(n) {}

  friend constexpr auto operator<=>(InstructionIndex,
                                    InstructionIndex) = default;

  constexpr value_type value() const { return index_; }

  constexpr InstructionIndex& operator+=(value_type n) {
    index_ += n;
    return *this;
  }

  friend constexpr InstructionIndex operator+(InstructionIndex lhs,
                                              difference_type rhs) {
    return lhs += rhs;
  }

  constexpr InstructionIndex& operator-=(value_type n) {
    index_ -= n;
    return *this;
  }

  friend constexpr InstructionIndex operator-(InstructionIndex lhs,
                                              difference_type rhs) {
    return lhs -= rhs;
  }

  friend constexpr difference_type operator-(InstructionIndex lhs,
                                             InstructionIndex rhs) {
    return lhs.value() - rhs.value();
  }

 private:
  value_type index_;
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_INDEX_H
