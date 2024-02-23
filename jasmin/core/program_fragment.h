#ifndef JASMIN_CORE_PROGRAM_FRAGMENT_H
#define JASMIN_CORE_PROGRAM_FRAGMENT_H

#include <cstdint>
#include <string>
#include <string_view>

#include "jasmin/core/function.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/internal/function_forward.h"
#include "nth/container/flyweight_map.h"
#include "nth/debug/debug.h"
#include "nth/io/deserialize/deserialize.h"
#include "nth/io/serialize/serialize.h"
#include "nth/utility/iterator_range.h"

namespace jasmin {

template <InstructionSetType Set>
struct ProgramFragment;

namespace internal {

struct ProgramFragmentBase {
  struct function_identifier {
    uint32_t value() const { return value_; }

    constexpr friend bool operator==(function_identifier,
                                     function_identifier) = default;

    template <typename H>
    friend H AbslHashValue(H h, function_identifier f) {
      return H::combine(std::move(h), f.value_);
    }

   private:
    template <InstructionSetType>
    friend struct ::jasmin::ProgramFragment;

    constexpr function_identifier(uint32_t n) : value_(n) {}

    uint32_t value_;
  };
};

}  // namespace internal

// A `ProgramFragment` represents a collection of functions, each with a unique
// name. Functions within this collection may call other functions in the same
// program fragment or within different program fragments.
template <InstructionSetType Set>
struct ProgramFragment : internal::ProgramFragmentBase {
  struct declare_result {
    function_identifier identifier;
    Function<Set>& function;
  };

  // Declares a function owned by this `ProgramFragment` with the given name and
  // signature (number of inputs and outputs).
  declare_result declare(std::string const& name, uint32_t inputs,
                         uint32_t outputs);

  // Returns a reference to the function declared with the given name or
  // identifier. Behavior is undefined if no such function exists.
  [[nodiscard]] Function<Set> const& function(std::string const& name) const;
  [[nodiscard]] Function<Set>& function(std::string const& name);
  [[nodiscard]] Function<Set> const& function(function_identifier id) const;
  [[nodiscard]] Function<Set>& function(function_identifier id);

  template <nth::io::serializer_with_context<FunctionRegistry> S>
  friend nth::io::serializer_result_type<S> NthSerialize(
      S& s, ProgramFragment const& p) {
    using result_type = nth::io::serializer_result_type<S>;
    if (not nth::io::write_integer(s, p.functions_.size())) {
      return result_type(false);
    }
    auto& registry = s.context(nth::type<FunctionRegistry>);
    for (auto const& [name, fn] : p.functions_) {
      registry.register_function(p, fn);
      if (not nth::io::write_integer(s, name.size())) {
        return result_type(false);
      }
      if (not s.write(std::span<std::byte const>(
              reinterpret_cast<std::byte const*>(name.data()), name.size()))) {
        return result_type(false);
      }
    }

    result_type result(true);
    for (auto const& [name, fn] : p.functions_) {
      result = nth::io::serialize(s, fn);
      if (not result) { return result; }
    }
    return result;
  }

  template <nth::io::deserializer_with_context<FunctionRegistry> D>
  friend nth::io::deserializer_result_type<D> NthDeserialize(
      D& d, ProgramFragment& p) {
    using result_type = nth::io::deserializer_result_type<D>;
    size_t size;
    if (not nth::io::read_integer(d, size)) { return result_type(false); }

    std::vector<Function<Set>*> fns;
    fns.reserve(size);

    auto& registry = d.context(nth::type<FunctionRegistry>);
    for (uint32_t i = 0; i < size; ++i) {
      size_t name_size;
      if (not nth::io::read_integer(d, name_size)) {
        return result_type(false);
      }
      std::string name(name_size, '\0');
      if (not d.read(std::span<std::byte>(
              reinterpret_cast<std::byte*>(name.data()), name.size()))) {
        return result_type(false);
      }

      auto [iter, inserted] = p.functions_.try_emplace(name);
      if (inserted) { registry.register_function(p, iter->second); }
      fns.push_back(&iter->second);
    }

    result_type result(true);
    for (Function<Set>* fn : fns) {
      result = nth::io::deserialize(d, *fn);
      if (not result) { return result; }
    }
    return result;
  }

  // Returns the number of functions managed by this `ProgramFragment`.
  size_t function_count() const { return functions_.size(); }

  auto functions() const {
    return nth::iterator_range(functions_.begin(), functions_.end());
  }

 private:
  nth::flyweight_map<std::string, Function<Set>> functions_;
};

template <InstructionSetType Set>
ProgramFragment<Set>::declare_result ProgramFragment<Set>::declare(
    std::string const& name, uint32_t inputs, uint32_t outputs) {
  auto [iter, inserted] = functions_.try_emplace(name, inputs, outputs);
  return declare_result{
      .identifier = function_identifier(functions_.index(iter)),
      .function   = iter->second,
  };
}

template <InstructionSetType Set>
Function<Set>& ProgramFragment<Set>::function(std::string const& name) {
  auto iter = functions_.find(name);
  NTH_REQUIRE((v.harden), iter != functions_.end());
  return iter->second;
}

template <InstructionSetType Set>
Function<Set> const& ProgramFragment<Set>::function(
    std::string const& name) const {
  auto iter = functions_.find(name);
  NTH_REQUIRE((v.harden), iter != functions_.end());
  return iter->second;
}

template <InstructionSetType Set>
Function<Set>& ProgramFragment<Set>::function(function_identifier id) {
  return functions_.from_index(id.value()).second;
}

template <InstructionSetType Set>
Function<Set> const& ProgramFragment<Set>::function(
    function_identifier id) const {
  return functions_.from_index(id.value()).second;
}

}  // namespace jasmin

#endif  // JASMIN_CORE_PROGRAM_FRAGMENT_H
