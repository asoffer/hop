#ifndef JASMIN_CORE_INTERNAL_FRAME_H
#define JASMIN_CORE_INTERNAL_FRAME_H

#include <cstdint>
#include <cstddef>

#include "jasmin/core/value.h"

namespace jasmin::internal {
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

}  // namespace jasmin::internal

#endif  // JASMIN_CORE_INTERNAL_FRAME_H
