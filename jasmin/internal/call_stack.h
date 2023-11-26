#ifndef JASMIN_INTERNAL_CALL_STACK_H
#define JASMIN_INTERNAL_CALL_STACK_H

#include <cstdint>
#include <cstring>

#include "jasmin/value.h"

namespace jasmin::internal {

inline constexpr bool EmptyCallStack(uint64_t cap_and_left) {
  return (cap_and_left >> 32) - (cap_and_left & 0xffffffff) == 2;
}

struct FrameBase {
  Value const *ip;
};

template <typename StateType>
struct Frame : FrameBase {
  StateType state;
};

template <>
struct Frame<void> : FrameBase {};

using exec_fn_type = void (*)(Value *, size_t, Value const *, FrameBase *,
                              uint64_t);

template <size_t N>
void ReallocateCallStack(Value *value_stack_head, size_t capacity,
                         Value const *ip, FrameBase *call_stack,
                         uint64_t cap_and_left) {
  uint32_t size = cap_and_left >> 32;
  auto *ptr     = static_cast<FrameBase *>(operator new(N *(size << 1)));
  auto *old_ptr = call_stack - (size - 1);
  std::memcpy(ptr, old_ptr, N * size);
  operator delete(old_ptr);

  NTH_ATTRIBUTE(tailcall)
  return ip->template as<exec_fn_type>()(value_stack_head, capacity, ip,
                                         ptr + (size - 1),
                                         cap_and_left * 2 | size);
}

}  // namespace jasmin::internal

#endif  // JASMIN_INTERNAL_CALL_STACK_H
