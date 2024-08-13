#include "jasmin/compile/compiled_function.h"

#include <cstddef>
#include <cstdint>
#include <span>

#include "nth/debug/debug.h"
#include "nth/memory/bytes.h"

namespace jasmin {

CompiledFunction::operator std::span<std::byte const>() const {
  return nth::byte_range(content_);
}

void CompiledFunction::write(std::initializer_list<uint8_t> instructions) {
  for (char c : instructions) { content_.push_back(c); }
}

void CompiledFunction::write_at_impl(size_t offset, uint32_t n) {
  NTH_REQUIRE((harden), n + sizeof(uint32_t) <= content_.size());
  std::memcpy(content_.data() + offset, &n, sizeof(n));
}

}  // namespace jasmin
