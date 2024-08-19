#include "hop/core/metadata.h"

#include <cstdint>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "hop/core/instruction.h"
#include "hop/core/internal/instruction_traits.h"
#include "hop/core/value.h"
#include "nth/debug/debug.h"

namespace hop {

InstructionMetadata const& InstructionSetMetadata::metadata(
    uint16_t opcode) const {
  NTH_REQUIRE((harden), opcode < metadata_.size());
  return metadata_[opcode];
}

Value InstructionSetMetadata::function(uint16_t opcode) const {
  return functions_[opcode];
}

uint16_t InstructionSetMetadata::opcode(Value f) const {
  auto iter = opcode_.find(f.as<internal::exec_fn_type>());
  NTH_REQUIRE((harden), iter != opcode_.end());
  return iter->second;
}

InstructionSetMetadata::InstructionSetMetadata(
    std::vector<InstructionMetadata> metadata,
    std::vector<internal::exec_fn_type> fns)
    : metadata_(std::move(metadata)), functions_(std::move(fns)) {
  for (auto const& f : functions_) { opcode_.emplace(f, opcode_.size()); }
}

}  // namespace hop
