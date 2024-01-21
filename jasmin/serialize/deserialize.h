#ifndef JASMIN_SERIALIZE_DESERIALIZE_H
#define JASMIN_SERIALIZE_DESERIALIZE_H

#include <array>
#include <concepts>

#include "jasmin/core/function.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/metadata.h"
#include "jasmin/core/program.h"
#include "jasmin/core/value.h"
#include "jasmin/serialize/reader.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {

// Deserializes a `Program` from the reader `r` into `p`. Returns a bool
// indicating whether or not deserialization succeeded. On failure, `p` will be
// in a valid, but unspecified state.
template <InstructionSetType Set, Reader R>
bool Deserialize(R& r, Program<Set>& p);

// Implementation

namespace internal {

template <typename T>
bool ReadImpl(Reader auto& r, Function<>& f) {
  T value;
  if (not JasminDeserialize(r, value)) { return false; }
  f.raw_append(value);
  return true;
}

template <InstructionType I, Reader R>
bool InstructionDeserializer(R& r, Function<>& f) {
  if constexpr (nth::type<I> == nth::type<Return>) {
  } else if constexpr (nth::type<I> == nth::type<Call>) {
    return ReadImpl<InstructionSpecification>(r, f);
  } else if constexpr (nth::type<I> == nth::type<Jump> or
                       nth::type<I> == nth::type<JumpIf>) {
    return ReadImpl<size_t>(r, f);
  } else if constexpr (requires { I::execute; }) {
    constexpr auto params = nth::type<decltype(I::execute)>.parameters();
    return params.template drop<params.size() - ImmediateValueCount<I>()>()
        .reduce([&](auto... ts) {
          return (jasmin::internal::ReadImpl<nth::type_t<ts>>(r, f) and ...);
        });
  } else {
    constexpr auto params = nth::type<decltype(I::consume)>.parameters();
    return params.template drop<params.size() - ImmediateValueCount<I>()>()
        .reduce([&](auto... ts) {
          return (jasmin::internal::ReadImpl<nth::type_t<ts>>(r, f) and ...);
        });
  }
  return true;
}

template <InstructionSetType Set, Reader R>
inline constexpr std::array InstructionDeserializers =
    Set::instructions.reduce([](auto... ts) {
      return std::array{InstructionDeserializer<nth::type_t<ts>, R>...};
    });

template <InstructionSetType Set, Reader R>
bool Deserialize(R& r, Function<Set>& f) {
  uint16_t length;
  auto c = jasmin::ReadLengthPrefix(r, length);
  if (not c) { return false; }
  while (r.size() != 0) {
    uint16_t op_code;
    if (not jasmin::ReadFixed(r, op_code)) { return false; }
    f.raw_append(Metadata<Set>().function(op_code));
    if (not InstructionDeserializers<Set, R>[op_code](r, f)) { return false; }
  }
  return r.cursor() - *c == length;
}

}  // namespace internal

template <InstructionSetType Set, Reader R>
bool Deserialize(R& r, Program<Set>& p) {
  auto start = r.cursor();
  std::vector<std::pair<std::string, typename R::cursor_type>> cursors;
  uint32_t function_count;
  if (not jasmin::ReadFixed(r, function_count)) { return false; }
  cursors.reserve(function_count);

  for (size_t i = 0; i < function_count; ++i) {
    uint32_t offset;
    auto c = r.cursor();
    if (not jasmin::ReadFixed(r, offset)) { return false; }

    uint32_t char_count;
    if (not jasmin::ReadFixed(r, char_count)) { return false; }
    std::string name(char_count, '\0');
    if (not r.read(std::span<std::byte>(
            reinterpret_cast<std::byte*>(name.data()), char_count))) {
      return false;
    }

    uint32_t parameter_count, return_count;
    if (not jasmin::ReadInteger(r, parameter_count)) { return false; }
    if (not jasmin::ReadInteger(r, return_count)) { return false; }
    p.declare(name, parameter_count, return_count);
    cursors.emplace_back(std::move(name), c);
  }

  for (auto& [name, f] : p.functions()) {
    r.register_function(
        name, &const_cast<Function<>&>(static_cast<Function<> const&>(f)));
  }

  for (size_t i = 0; i < function_count; ++i) {
    uint32_t offset;
    r.read(cursors[i].second,
           std::span(reinterpret_cast<std::byte*>(&offset), sizeof(uint32_t)));
    if (r.cursor() - start != offset) { return false; }

    auto& fn = p.function(cursors[i].first);
    if (not internal::Deserialize(r, fn)) { return false; }
  }
  return r.size() == 0;
}

bool JasminDeserialize(Reader auto& r, std::integral auto& n) {
  return jasmin::ReadInteger(r, n);
}

inline bool JasminDeserialize(Reader auto& r, InstructionSpecification& spec) {
  return jasmin::ReadInteger(r, spec.parameters) and
         jasmin::ReadInteger(r, spec.returns);
}

}  // namespace jasmin

#endif  // JASMIN_SERIALIZE_DESERIALIZE_H
