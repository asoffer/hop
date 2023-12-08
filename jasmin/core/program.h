#ifndef JASMIN_CORE_PROGRAM_H
#define JASMIN_CORE_PROGRAM_H

#include <string>
#include <string_view>

#include "absl/container/node_hash_map.h"
#include "jasmin/core/function.h"
#include "jasmin/core/instruction.h"
#include "nth/debug/debug.h"

namespace jasmin {

// A `Program` represents a collection of functions closed under the caller ->
// callee relationship, with a unique entry-point. It is the responsibility of
// the user to ensure that any function transitively invocable via functions in
// a program are also in the same program. Functions transitively invocable are
// assumed to be owned solely by the `Program`. They may be modified (by an
// optimizer, or a debugger), and could therefore cause unsupported, unexpected,
// or undefined behavior if this requirement is violated.
template <InstructionSetType Set>
struct Program {
  Function<Set>& declare(std::string_view name, uint32_t inputs,
                         uint32_t outputs);

  Function<Set> const& function(std::string_view name) const;
  Function<Set>& function(std::string_view name);

 private:
  absl::node_hash_map<std::string, Function<Set>> functions_;
};

template <InstructionSetType Set>
Function<Set>& Program<Set>::declare(std::string_view name, uint32_t inputs,
                                     uint32_t outputs) {
  auto [iter, inserted] = functions_.try_emplace(name, inputs, outputs);
  return iter->second;
}

template <InstructionSetType Set>
Function<Set>& Program<Set>::function(std::string_view name) {
  auto iter = functions_.find(name);
  NTH_REQUIRE((v.harden), iter != functions_.end());
  return iter->second;
}

template <InstructionSetType Set>
Function<Set> const& Program<Set>::function(std::string_view name) const {
  auto iter = functions_.find(name);
  NTH_REQUIRE((v.harden), iter != functions_.end());
  return iter->second;
}

}  // namespace jasmin

#endif  // JASMIN_CORE_PROGRAM_H
