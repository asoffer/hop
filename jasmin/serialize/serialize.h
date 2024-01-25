#ifndef JASMIN_SERIALIZE_SERIALIZE_H
#define JASMIN_SERIALIZE_SERIALIZE_H

#include <array>
#include <concepts>
#include <span>

#include "jasmin/core/function.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/metadata.h"
#include "jasmin/core/program.h"
#include "jasmin/core/value.h"
#include "jasmin/serialize/writer.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {

// Serializes a `Program p` via the `Writer w`.
template <InstructionSetType Set, Writer W>
void Serialize(Program<Set> const& p, W& w);

// Implementation

void JasminSerialize(Writer auto& w, std::integral auto n) {
  if constexpr (nth::type<decltype(n)> == nth::type<bool>) {
    jasmin::WriteFixed(w, n);
  } else {
    jasmin::WriteInteger(w, n);
  }
}
void JasminSerialize(Writer auto& w, std::floating_point auto f) {
  jasmin::WriteFixed(w, f);
}

void JasminSerialize(Writer auto& w, std::byte b) { w.write(b); }

void JasminSerialize(Writer auto&, Function<> const*) { NTH_UNIMPLEMENTED(); }

void JasminSerialize(Writer auto& w, InstructionSpecification spec) {
  jasmin::WriteInteger(w, spec.parameters);
  jasmin::WriteInteger(w, spec.returns);
}

namespace internal {

template <Writer W, InstructionType I>
void InstructionSerializer(std::span<Value const> v, W& w) {
  if constexpr (nth::type<I> == nth::type<Return>) {
  } else if constexpr (nth::type<I> == nth::type<Call>) {
    JasminSerialize(w, v[0].as<InstructionSpecification>());
  } else if constexpr (nth::type<I> == nth::type<Jump> or
                       nth::type<I> == nth::type<JumpIf>) {
    JasminSerialize(w, v[0].as<size_t>());
  } else {
    constexpr auto params = [] {
      if constexpr (requires { I::execute; }) {
        return nth::type<decltype(I::execute)>.parameters();
      } else {
        return nth::type<decltype(I::consume)>.parameters();
      }
    }();
    params.template drop<params.size() - ImmediateValueCount<I>()>().reduce(
        [&](auto... ts) {
          size_t i = 0;
          (JasminSerialize(w, v[i++].template as<nth::type_t<ts>>()), ...);
        });
  }
}

template <Writer W, InstructionSetType Set>
inline constexpr std::array InstructionSerializers =
    Set::instructions.reduce([](auto... ts) {
      return std::array{InstructionSerializer<W, nth::type_t<ts>>...};
    });

template <typename T>
std::span<std::byte const, sizeof(T)> View(T const& value) {
  return std::span<std::byte const, sizeof(T)>(
      reinterpret_cast<std::byte const*>(std::addressof(value)));
}

}  // namespace internal

template <InstructionSetType Set, Writer W>
void Serialize(Program<Set> const& p, W& w) {
  auto start = w.cursor();
  absl::flat_hash_map<std::string_view, typename W::cursor_type> locations;
  jasmin::WriteFixed(w, static_cast<uint32_t>(p.function_count()));
  for (auto const& [name, fn] : p.functions()) {
    locations.emplace(name, w.allocate(sizeof(uint32_t)));
    jasmin::WriteFixed(w, static_cast<uint32_t>(name.size()));
    w.write(std::span<std::byte const>(
        reinterpret_cast<std::byte const*>(name.data()), name.size()));
    jasmin::WriteInteger(w, fn.parameter_count());
    jasmin::WriteInteger(w, fn.return_count());
  }
  for (auto const& [name, fn] : p.functions()) {
    auto iter = locations.find(name);
    NTH_REQUIRE(iter != locations.end());
    uint32_t distance = w.cursor() - start;
    w.write(iter->second,
            std::span(reinterpret_cast<std::byte const*>(&distance),
                      sizeof(distance)));
    auto const& set_metadata  = Metadata<Set>();
    typename W::cursor_type c = w.allocate(sizeof(uint16_t));
    std::span insts           = fn.raw_instructions();
    while (not insts.empty()) {
      uint16_t index = set_metadata.opcode(insts[0]);
      jasmin::WriteFixed(w, index);
      auto immediate_value_count =
          set_metadata.metadata(index).immediate_value_count;
      internal::InstructionSerializers<W, Set>[index](
          insts.subspan(1, immediate_value_count), w);
      insts = insts.subspan(immediate_value_count + 1);
    }
    WritePrefixedLength<uint16_t>(w, c);
  }
}

}  // namespace jasmin

#endif  // JASMIN_SERIALIZE_SERIALIZE_H
