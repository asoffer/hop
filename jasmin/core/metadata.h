#ifndef JASMIN_CORE_METADATA_H
#define JASMIN_CORE_METADATA_H

#include <cstddef>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/internal/instruction_traits.h"
#include "jasmin/core/value.h"
#include "nth/base/indestructible.h"

namespace jasmin {

struct InstructionMetadata {
  // A human-readable name for the instruction, with no guarantees on
  // uniqueness.
  std::string_view name;

  // The number of immediate values required by the instruction.
  size_t immediate_value_count;

  // The number of parameters accepted by the instruction if this is known
  // statically, and zero otherwise.
  size_t parameter_count;

  // The number of returns provided by the instruction if this is known
  // statically, and zero otherwise.
  size_t return_count;

  // Whether or not the instruction consumes its input (`true`) or leaves its
  // input on the stack (`false`).
  bool consumes_input;
};

// Represents metadata collectively about all instructions in an entire
// instruction set. In particular, instructions do not have a dedicated op-code
// except in relation to a specific instruction set, so the op-code cannot be
// exposed as part of an `InstructionMetadata`.
struct InstructionSetMetadata {
  size_t size() const { return metadata_.size(); }

  // Returns the metadata associated the instruction indexed by the given
  // `opcode`.
  InstructionMetadata const& metadata(uint16_t opcode) const;

  // Returns the function to be invoked by the interpreter when evaluating the
  // instruction indexed by the given `opcode`.
  Value function(uint16_t opcode) const;

  // Returns the opcode associated with function `f` used by the interpreter
  // when evaluating the associated instruction. Functionally, this is the
  // inverse of the member function named `function`)
  uint16_t opcode(Value f) const;

 private:
  template <InstructionSetType Set>
  friend InstructionSetMetadata const& Metadata();

  explicit InstructionSetMetadata(std::vector<InstructionMetadata> metadata,
                                  std::vector<internal::exec_fn_type> fns);

  std::vector<InstructionMetadata> metadata_;
  absl::flat_hash_map<internal::exec_fn_type, size_t> opcode_;
  std::vector<internal::exec_fn_type> functions_;
};

// `Metadata<Set>` evaluates to a reference to an eternal
// `InstructionSetMetadata` object holding metadata about the instruction set
// `Set`.
template <InstructionSetType Set>
InstructionSetMetadata const& Metadata() {
  static nth::indestructible<InstructionSetMetadata> metadata =
      Set::instructions.reduce([](auto... is) {
        return InstructionSetMetadata(
            {InstructionMetadata{
                .name                  = InstructionName<nth::type_t<is>>(),
                .immediate_value_count = ImmediateValueCount<nth::type_t<is>>(),
                .parameter_count       = ParameterCount<nth::type_t<is>>(),
                .return_count          = ReturnCount<nth::type_t<is>>(),
                .consumes_input        = ConsumesInput<nth::type_t<is>>(),
            }...},
            {&nth::type_t<is>::template ExecuteImpl<Set>...});
      });
  return *metadata;
}

}  // namespace jasmin

#endif  // JASMIN_CORE_METADATA_H
