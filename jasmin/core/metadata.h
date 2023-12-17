#ifndef JASMIN_CORE_METADATA_H
#define JASMIN_CORE_METADATA_H

#include <cstddef>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/internal/instruction_traits.h"
#include "jasmin/core/value.h"
#include "nth/debug/debug.h"

namespace jasmin {

struct InstructionMetadata {
  size_t immediate_value_count;
  size_t parameter_count;
  Value function;
};

struct InstructionSetMetadata {
  explicit InstructionSetMetadata(std::vector<InstructionMetadata> metadata)
      : metadata_(std::move(metadata)) {
    for (auto const& m : metadata_) {
      op_code_.emplace(m.function.as<internal::exec_fn_type>(),
                       op_code_.size());
    }
  }

  size_t size() const { return metadata_.size(); }

  InstructionMetadata metadata(size_t op_code) const {
    NTH_REQUIRE((v.harden), op_code < metadata_.size());
    return metadata_[op_code];
  }

  uint64_t opcode(Value f) const {
    auto iter = op_code_.find(f.as<internal::exec_fn_type>());
    NTH_REQUIRE((v.harden), iter != op_code_.end());
    return iter->second;
  }

  InstructionMetadata metadata_by_function(Value f) const {
    return metadata(opcode(f));
  }

 private:
  std::vector<InstructionMetadata> metadata_;
  absl::flat_hash_map<internal::exec_fn_type, size_t> op_code_;
};

namespace internal {

// `Metadata<I>` evaluates to an `InstructionMetadata` object holding metadata
// about the instruction `I`.
template <InstructionType I, InstructionSetType Set>
InstructionMetadata Metadata = InstructionMetadata{
    .immediate_value_count = ImmediateValueCount<I>(),
    .parameter_count       = ParameterCount<I>(),
    .function              = Value(&I::template ExecuteImpl<Set>),
};

}  // namespace internal

// `Metadata<Set>` evaluates to an `InstructionSetMetadata` object holding
// metadata about the instruction set `Set`.
template <InstructionSetType Set>
InstructionSetMetadata Metadata = Set::instructions.reduce([](auto... is) {
  return InstructionSetMetadata({internal::Metadata<nth::type_t<is>, Set>...});
});

template <InstructionType I, InstructionSetType Set>
Value InstructionFunction = Value(&I::template ExecuteImpl<Set>);

}  // namespace jasmin

#endif  // JASMIN_CORE_METADATA_H
