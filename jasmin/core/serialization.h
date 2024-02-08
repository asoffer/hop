#ifndef JASMIN_CORE_SERIALIZATION_H
#define JASMIN_CORE_SERIALIZATION_H

#include <array>
#include <concepts>
#include <span>

#include "jasmin/core/function.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/program.h"
#include "nth/io/serialize/deserialize.h"
#include "nth/io/serialize/serialize.h"

namespace jasmin {
namespace internal {

template <typename S, InstructionType>
requires std::is_lvalue_reference_v<S>
bool InstructionSerializer(S, std::span<Value const>);
template <typename D, InstructionType>
requires std::is_lvalue_reference_v<D>
bool InstructionDeserializer(D, Function<> &);

}  // namespace internal

struct ProgramSerializer {
  void register_function(Function<> const &f) {
    registered_functions_.emplace(&f, registered_functions_.size());
  }

  friend bool NthSerialize(std::derived_from<ProgramSerializer> auto &s,
                           Function<> const *f) {
    auto iter = s.registered_functions_.find(f);
    if (iter == s.registered_functions_.end()) { return false; }
    return nth::io::serialize_fixed(s, static_cast<uint32_t>(iter->second));
  }

  template <InstructionSetType Set>
  friend bool NthSerialize(std::derived_from<ProgramSerializer> auto &s,
                    Function<Set> const &fn) {
    if (not nth::io::serialize_integer(s, fn.parameter_count())) {
      return false;
    }
    if (not nth::io::serialize_integer(s, fn.return_count())) { return false; }
    auto const &set_metadata                 = Metadata<Set>();
    std::optional c = s.allocate(sizeof(uint32_t));
    if (not c) { return false; }
    std::span insts = fn.raw_instructions();

    static constexpr auto Serializers =
        Set::instructions.reduce([](auto... ts) {
          return std::array<bool (*)(decltype(s), std::span<Value const>),
                            sizeof...(ts)>{
              internal::InstructionSerializer<decltype(s), nth::type_t<ts>>...};
        });

    while (not insts.empty()) {
      uint16_t index = set_metadata.opcode(insts[0]);
      if (not nth::io::serialize_fixed(s, index)) { return false; }
      auto immediate_value_count =
          set_metadata.metadata(index).immediate_value_count;
      Serializers[index](s, insts.subspan(1, immediate_value_count));
      insts = insts.subspan(immediate_value_count + 1);
    }
    uint16_t distance = static_cast<uint16_t>(s.cursor() - *c);
    return s.write_at(*c, nth::bytes(distance));
  }

 private:
  absl::flat_hash_map<Function<> const *, size_t> registered_functions_;
};

struct ProgramDeserializer {
  friend bool NthDeserialize(std::derived_from<ProgramDeserializer> auto &d,
                             std::integral auto &x) {
    return nth::io::deserialize_integer(d, x);
  }

  friend bool NthDeserialize(std::derived_from<ProgramDeserializer> auto &d,
                             std::floating_point auto &x) {
    return nth::io::deserialize_fixed(d, x);
  }

  friend bool NthDeserialize(std::derived_from<ProgramDeserializer> auto &d,
                             Function<> *&fn) {
    uint32_t index;
    if (not nth::io::deserialize_fixed(d, index)) { return false; }
    if (index >= d.registered_functions_.size()) { return false; }
    fn = d.registered_functions_[index];
    return true;
  }

  template <InstructionSetType Set>
  friend bool NthDeserialize(std::derived_from<ProgramDeserializer> auto &d,
                             Function<Set> &fn) {
    uint32_t parameter_count, return_count;
    if (not nth::io::deserialize_integer(d, parameter_count)) { return false; }
    if (not nth::io::deserialize_integer(d, return_count)) { return false; }
    fn = Function<Set>(parameter_count, return_count);

    uint32_t expected_length;
    auto c = d.cursor();
    if (not nth::io::deserialize_fixed(d, expected_length)) { return false; }

    auto const &set_metadata = Metadata<Set>();
    static constexpr auto Deserializers =
        Set::instructions.reduce([](auto... ts) {
          return std::array<bool (*)(decltype(d), Function<> &), sizeof...(ts)>{
              internal::InstructionDeserializer<decltype(d),
                                                nth::type_t<ts>>...};
        });

    while (d.cursor() - c < expected_length) {
      uint16_t opcode;
      if (not nth::io::deserialize_fixed(d, opcode)) { return false; }

      fn.raw_append(set_metadata.function(opcode));
      if (not Deserializers[opcode](d, fn)) { return false; }
    }

    return d.cursor() - c == expected_length;
  }

  std::span<Function<> *> registered_functions() {
    return registered_functions_;
  }
  void register_function(Function<> &f) { registered_functions_.push_back(&f); }

 private:
  std::vector<Function<> *> registered_functions_;
};

namespace internal {

template <typename S, InstructionType I>
requires std::is_lvalue_reference_v<S>
bool InstructionSerializer(S s, std::span<Value const> v) {
  if constexpr (nth::type<I> == nth::type<Return>) {
    return true;
  } else if constexpr (nth::type<I> == nth::type<Call>) {
    return nth::io::serialize(s, v[0].as<InstructionSpecification>());
  } else if constexpr (nth::any_of<I, Jump, JumpIf, JumpIfNot>) {
    return nth::io::serialize_integer(s, v[0].as<ptrdiff_t>());
  } else {
    constexpr auto params = [] {
      if constexpr (requires { I::execute; }) {
        return nth::type<decltype(I::execute)>.parameters();
      } else {
        return nth::type<decltype(I::consume)>.parameters();
      }
    }();
    return params
        .template drop<2 + (::jasmin::FunctionState<I>() != nth::type<void>)>()
        .reduce([&](auto... ts) {
          size_t i = 0;
          return (
              nth::io::serialize(s, v[i++].template as<nth::type_t<ts>>()) and
              ...);
        });
  }
}

template <typename D, InstructionType I>
requires std::is_lvalue_reference_v<D>
bool InstructionDeserializer(D d, Function<> &fn) {
  if constexpr (nth::type<I> == nth::type<Return>) {
    return true;
  } else if constexpr (nth::type<I> == nth::type<Call>) {
    InstructionSpecification spec;
    if (not nth::io::deserialize(d, spec)) { return false; }
    fn.raw_append(spec);
    return true;
  } else if constexpr (nth::type<I> == nth::type<Jump> or
                       nth::type<I> == nth::type<JumpIf>) {
    ptrdiff_t amount;
    if (not nth::io::deserialize(d, amount)) { return false; }
    fn.raw_append(amount);
    return true;
  } else {
    constexpr bool HasFunctionState = internal::HasFunctionState<I>;
    constexpr auto parameters =
        internal::InstructionFunctionType<I>().parameters();
    return parameters.template drop<HasFunctionState + 2>().reduce(
        [&](auto... ts) {
          return ([&](auto t) {
            nth::type_t<t> value;
            if (not nth::io::deserialize(d, value)) { return false; }
            fn.raw_append(value);
            return true;
          }(ts) and
                  ...);
        });
  }
}

}  // namespace internal
}  // namespace jasmin

#endif  // JASMIN_CORE_SERIALIZATION_H
