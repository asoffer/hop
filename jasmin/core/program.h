#ifndef JASMIN_CORE_PROGRAM_H
#define JASMIN_CORE_PROGRAM_H

#include <cstdint>
#include <string>
#include <string_view>

#include "jasmin/core/function.h"
#include "jasmin/core/instruction.h"
#include "nth/container/flyweight_map.h"
#include "nth/debug/debug.h"
#include "nth/utility/iterator_range.h"

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
  struct function_identifier {
    uint32_t value() const { return value_; }

    constexpr friend bool operator==(function_identifier,
                                     function_identifier) = default;

    template <typename H>
    friend H AbslHashValue(H h, function_identifier f) {
      return H::combine(std::move(h), f.value_);
    }

   private:
    friend Program;

    constexpr function_identifier(uint32_t n) : value_(n) {}

    uint32_t value_;
  };

  struct declare_result {
    function_identifier identifier;
    Function<Set>& function;
  };

  // Declares a function owned by this `Program` with the given name and
  // signature (number of inputs and outputs).
  declare_result declare(std::string const& name, uint32_t inputs,
                         uint32_t outputs);

  // Returns a reference to the function declared with the given name or
  // identifier. Behavior is undefined if no such function exists.
  [[nodiscard]] Function<Set> const& function(std::string const& name) const;
  [[nodiscard]] Function<Set>& function(std::string const& name);
  [[nodiscard]] Function<Set> const& function(function_identifier id) const;
  [[nodiscard]] Function<Set>& function(function_identifier id);

  // Returns the number of functions managed by this `Program`.
  size_t function_count() const { return functions_.size(); }

  auto functions() const {
    return nth::iterator_range(functions_.begin(), functions_.end());
  }

 private:
  nth::flyweight_map<std::string, Function<Set>> functions_;
};

template <InstructionSetType Set>
Program<Set>::declare_result Program<Set>::declare(std::string const& name,
                                                   uint32_t inputs,
                                                   uint32_t outputs) {
  auto [iter, inserted] = functions_.try_emplace(name, inputs, outputs);
  return declare_result{
      .identifier = function_identifier(functions_.index(iter)),
      .function   = iter->second,
  };
}

template <InstructionSetType Set>
Function<Set>& Program<Set>::function(std::string const& name) {
  auto iter = functions_.find(name);
  NTH_REQUIRE((v.harden), iter != functions_.end());
  return iter->second;
}

template <InstructionSetType Set>
Function<Set> const& Program<Set>::function(std::string const& name) const {
  auto iter = functions_.find(name);
  NTH_REQUIRE((v.harden), iter != functions_.end());
  return iter->second;
}

template <InstructionSetType Set>
Function<Set>& Program<Set>::function(function_identifier id) {
  return functions_.from_index(id.value()).second;
}

template <InstructionSetType Set>
Function<Set> const& Program<Set>::function(function_identifier id) const {
  return functions_.from_index(id.value()).second;
}

}  // namespace jasmin

#endif  // JASMIN_CORE_PROGRAM_H
