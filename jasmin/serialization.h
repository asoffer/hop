#ifndef JASMIN_SERIALIZATION_H
#define JASMIN_SERIALIZATION_H

#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <string_view>

#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "jasmin/internal/debug.h"
#include "jasmin/internal/type_list.h"

// This file contains a mechanism by which Jasmin functions can be serialized
// and deserialized. Instructions with immediate arguments will have their
// arguments serialized and deserialized as if via `std::memcpy` if no
// customization is provided. Users can customize this behavior by specifying
// three things in their binary:
//
//   1. A type alias for `serialization_state`.
//   2. A static member function named `serialize` which accepts a
//      `jasmin::Serializer&` as its first argument, a `std::span<Value const>`
//      as its second argument, and a `serialization_state&` as its third
//      argument (or no third argument if `serialization_state` is void. This
//      function will be invoked to serialize the immediate arguments provided
//      in the second argument.
//   3. A static member function named `deserialize` which accepts a
//      `jasmin::Deserializer&` as its first argument, a `std::span<Value>` as
//      its second argument, and a `serialization_state&` as its third argument
//      (or no third argument if `serialization_state` is void. This function
//      will be invoked to deserialize into the immediate arguments provided as
//      the second argument.
//
// Users are required to ensure that serialization followed by deserialization
// produces equivalent immediate values.
//
// If any instruction in a function requires state to be [de]serialized, a
// reference to a state object must be passed in to both the `Serialize` and
// `Deserialize` functions. This state object must implement a function template
// named `JasminSerializationState` findable via argument-dependent lookup with
// the signature
// ```
// template <typename T>
// T& JasminSerializationState(MyStateType &);
// ```
// and be able to accept any type argument for `T` which may be the
// `serialization_state` for an instruction.
//
// Please note, each instructions state is determined by type. This means that
// if two instructions share the same `serialization_state` type, they will use
// the same object for stateful [de]serialization. While this can be a useful
// feature, it is not always what you want. If you need to ensure state is not
// shared between instructions, please consider wrapping the state type in a
// type you control so no other instructions accidentally share it.
namespace jasmin {
namespace internal {

// Forward declarations for [de]serialization implementations defined below.
template <int&..., InstructionSet Set, typename StateType>
void SerializeImpl(Function<Set> const&, std::string&, StateType*);
template <int&..., InstructionSet Set, typename StateType>
bool DeserializeImpl(std::string_view, Function<Set>&, StateType*);

// Internal type trait detecting whether the instruction `I` has a custom
// serializer and whether or not it is stateful.
template <typename I>
struct SerializationStateImpl {
  static constexpr bool present = false;
  using type                    = void;
};
template <typename I>
requires(requires {
  typename I::serialization_state;
}) struct SerializationStateImpl<I> {
  static constexpr bool present = true;

  using type = typename I::serialization_state;
};

}  // namespace internal

// Structure responsible for serializing immediate value arguments to
// instructions. Invocable with any `std::integral` value, or a `jasmin::Value`.
struct Serializer {
  void operator()(std::integral auto n) {
    char char_buffer[sizeof(n)];
    std::memcpy(char_buffer, &n, sizeof(n));
    buffer_.append(std::begin(char_buffer), std::end(char_buffer));
  }

  void operator()(Value v) {
    char char_buffer[ValueSize];
    // Using `Value::Store` here over `std::memcpy` to avoid dependence on
    // whether the `JASMIN_DEBUG` macro is defined.
    Value::Store(v, char_buffer, ValueSize);
    buffer_.append(std::begin(char_buffer), std::end(char_buffer));
  }

 private:
  template <int&..., InstructionSet Set, typename StateType>
  friend void internal::SerializeImpl(Function<Set> const& function,
                                      std::string& output, StateType*);

  explicit Serializer(std::string& buffer) : buffer_(buffer) {}
  std::string& buffer_;
};

// Structure responsible for deserializing immediate value arguments to
// instructions. Invocable with a reference to any `std::integral` value or a
// `jasmin::Value`. Deserializes a value into the reference parameter if
// possible, and returns whether deserialization succeeded.
struct Deserializer {
  bool operator()(std::integral auto& n) {
    if (buffer_.size() < sizeof(n)) { return false; }
    std::memcpy(&n, buffer_.data(), sizeof(n));
    buffer_.remove_prefix(sizeof(n));
    return true;
  }

  bool operator()(Value& v) {
    if (buffer_.size() < ValueSize) { return false; }
    // Using `Value::Load` here over `std::memcpy` to avoid dependence on
    // whether the `JASMIN_DEBUG` macro is defined.
    v = Value::Load(buffer_.data(), ValueSize);
    buffer_.remove_prefix(ValueSize);
    return true;
    }

 private:
  template <int&..., InstructionSet Set, typename StateType>
  friend bool internal::DeserializeImpl(std::string_view input,
                                        Function<Set>& function, StateType*);

  explicit Deserializer(std::string_view buffer) : buffer_(buffer) {}
  std::string_view buffer_;
};

// Computes the type used for serialization state if one exists and `void`
// otherwise.
template <Instruction I>
using SerializationStateFor =
    typename internal::SerializationStateImpl<I>::type;
template <Instruction I>

// Boolean value indicating whether the instruction has a custom serializer
// defined.
inline constexpr bool HasCustomSerializer =
    internal::SerializationStateImpl<I>::present;

namespace internal {

template <typename Set>
using SerializationStateList =
    Filter<NotVoid, Unique<Transform<SerializationStateFor,
                                     typename Set::jasmin_instructions*>>>;

template <InstructionSet Set>
using SerializationState =
    internal::Apply<std::tuple, internal::SerializationStateList<Set>>;

// Function responsible for serializing `I` either via `std::memcpy` or via a
// custom serialization mechanism defined on the type `I`.
template <Instruction I, typename SerializationState>
std::type_identity_t<void (*)(Serializer&, std::span<Value const>, void*)>
ImmediateValuesSerializer() {
  using state_type = SerializationStateFor<I>;
  return +[](Serializer& serializer, std::span<Value const> values,
             void* state) {
    if constexpr (std::is_void_v<state_type>) {
      if constexpr (HasCustomSerializer<I>) {
        I::serialize(serializer, values);
      } else {
        for (auto const& value : values) { serializer(value); }
      }
    } else {
      static_assert(HasCustomSerializer<I>);
      static_cast<void>(I::serialize(
          serializer, values,
          static_cast<SerializationState*>(state)->template get<state_type>()));
    }
  };
}

// Function responsible for deserializing `I` either via `std::memcpy` or via a
// custom deserialization mechanism defined on the type `I`.
template <Instruction I, typename SerializationState>
std::type_identity_t<bool (*)(Deserializer&, std::span<Value>, void*)>
ImmediateValuesDeserializer() {
  using state_type = SerializationStateFor<I>;
  return +[](Deserializer& deserializer, std::span<Value> values, void* state) {
    if constexpr (std::is_void_v<state_type>) {
      if constexpr (HasCustomSerializer<I>) {
        return I::deserialize(deserializer, values);
      } else {
        for (auto& value : values) {
          if (not deserializer(value)) { return false; }
        }
        return true;
      }
    } else {
      static_assert(HasCustomSerializer<I>);
      return I::deserialize(
          deserializer, values,
          static_cast<SerializationState*>(state)->template get<state_type>());
    }
  };
}

struct InstructionMapEntry {
  uintptr_t function;
  uint64_t op_code;
  uint32_t immediate_value_count;
  void (*serializer)(Serializer&, std::span<Value const>, void*);
  bool (*deserializer)(Deserializer&, std::span<Value>, void*);
};

template <InstructionSet Set, typename StateType>
auto InstructionMapImpl() {
  constexpr size_t InstructionCount =
      internal::Invoke<[]<typename... Is>() { return sizeof...(Is); },
                       typename Set::jasmin_instructions*>;
  std::array<InstructionMapEntry, InstructionCount> result;
  internal::ForEach<typename Set::jasmin_instructions*>([&]<typename I>() {
    result[Set::template OpCodeFor<I>()] = {
        .function = reinterpret_cast<uintptr_t>(
            Set::InstructionFunction(Set::template OpCodeFor<I>())),
        .op_code               = Set::template OpCodeFor<I>(),
        .immediate_value_count = ImmediateValueCount<I>(),
        .serializer            = ImmediateValuesSerializer<I, StateType>(),
        .deserializer          = ImmediateValuesDeserializer<I, StateType>(),
    };
  });

  std::sort(result.begin(), result.end(),
            [](InstructionMapEntry const& lhs, InstructionMapEntry const& rhs) {
              return lhs.function < rhs.function;
            });
  return result;
}

template <InstructionSet Set, typename StateType>
auto const& InstructionMap() {
  static auto const result = InstructionMapImpl<Set, StateType>();
  return result;
}

// Returns a reference to the `InstructionMapEntry` associated with the given
// function pointer.
template <InstructionSet Set, typename StateType>
InstructionMapEntry const& FindInstructionMetadata(Value exec) {
  using instruction_type   = decltype(&Call::template ExecuteImpl<Set>);
  auto const& Instructions = internal::InstructionMap<Set, StateType>();

  auto instruction_ptr = exec.as<instruction_type>();

  auto iter = std::partition_point(
      Instructions.begin(), Instructions.end(), [&](auto const& entry) {
        return entry.function < reinterpret_cast<uintptr_t>(instruction_ptr);
      });

  JASMIN_INTERNAL_DEBUG_ASSERT(iter != Instructions.end(),
                               "Instruction not found.\n");

  JASMIN_INTERNAL_DEBUG_ASSERT(
      iter->function == reinterpret_cast<uintptr_t>(instruction_ptr),
      "Instruction missing from `Set`.");
  static_assert(ValueSize == sizeof(iter->op_code));
  return *iter;
}

template <int&..., InstructionSet Set, typename StateType>
void SerializeImpl(Function<Set> const& function, std::string& output,
                   StateType* state) {
  Serializer serializer(output);

  auto instructions = function.raw_instructions();
  auto iter         = instructions.begin();
  while (iter != instructions.end()) {
    auto const& entry =
        internal::FindInstructionMetadata<Set, StateType>(*iter++);
    serializer(entry.op_code);
    size_t bytes_written_index = output.size();
    serializer(uint32_t{0xabababab});  // Leave space for the number of bytes written.
    serializer(entry.immediate_value_count);
    std::span<Value const> values(iter, iter + entry.immediate_value_count);
    entry.serializer(serializer, values, state);
    uint32_t bytes_written = output.size() - bytes_written_index;
    std::memcpy(output.data() + bytes_written_index, &bytes_written,
                sizeof(bytes_written));
    iter += entry.immediate_value_count;
  }
}

template <int&..., InstructionSet Set, typename StateType>
bool DeserializeImpl(std::string_view input, Function<Set>& function,
                     StateType* state) {
  JASMIN_INTERNAL_DEBUG_ASSERT(
      input.size() % ValueSize == 0,
      "Input must have a length that is a multiple of `jasmin::ValueSize`.");
  if (input.empty()) { return true; }

  auto const& Instructions = internal::InstructionMap<Set, StateType>();

  char const* p   = input.data();
  char const* end = input.data() + input.size();
  while (p != end) {
    uint64_t op_code;
    std::memcpy(&op_code, p, sizeof(op_code));
    p += sizeof(op_code);
    auto instruction_ptr = Set::InstructionFunction(op_code);

    auto iter = std::partition_point(
        Instructions.begin(), Instructions.end(), [&](auto const& entry) {
          return entry.function < reinterpret_cast<uintptr_t>(instruction_ptr);
        });

    JASMIN_INTERNAL_DEBUG_ASSERT(iter != Instructions.end(),
                                 "Instruction not found.\n");
    JASMIN_INTERNAL_DEBUG_ASSERT(
        iter->function == reinterpret_cast<uintptr_t>(instruction_ptr),
        "Instruction missing from `Set`.");

    uint32_t encoded_size, value_count;
    std::memcpy(&encoded_size, p, sizeof(encoded_size));
    std::memcpy(&value_count, p + sizeof(encoded_size), sizeof(value_count));

    if (encoded_size > end - p) { return false; }
    char const* q = p + sizeof(encoded_size) + sizeof(value_count);
    Deserializer deserializer(std::string_view(q, encoded_size));

    p += encoded_size;

    function.raw_append(instruction_ptr);
    for (size_t i = 0; i < value_count; ++i) {
      function.raw_append(Value::Uninitialized());
    }

    std::span<Value> values = function.raw_instructions();
    values = values.subspan(values.size() - value_count, value_count);
    if (not iter->deserializer(deserializer, values, state)) { return false; }
  }
  return true;
}

}  // namespace internal

// Serializes the instructions in `function` to `output`. The only guarantee
// provided is that if a function is serialized to some string and then that
// string is deserialized (see below) to some function, that the two functions
// will have the same instructions (i.e., with the same op-codes) and immediate
// values (serialized either via a custom serializer if one is provided for the
// instruction, or via `std::memcpy` otherwise).
//
// Note that it is safe to serialize and deserialize in separate binaries, or
// separate invocations of the same binary, provided they were compiled with
// identical versions of Jasmin with the same compilation arguments.
template <int&..., InstructionSet Set>
void Serialize(Function<Set> const& function, std::string& output) {
  internal::SerializeImpl(function, output, static_cast<void*>(nullptr));
}

// TODO: Valdiate that `state` satisfies the requirements with respect to `Set`.
template <int&..., InstructionSet Set, typename SerializationState>
void Serialize(Function<Set> const& function, std::string& output,
               SerializationState& state) {
  return internal::SerializeImpl(function, output, &state);
}

// Deserializes `input` into `function`. The string `input` must be a string
// that was serialized (or could have been serialized) by `Serialize` (defined
// above). See caveats in documentation above regarding the validity of
// serialization across binaries.
template <int&..., InstructionSet Set>
bool Deserialize(std::string_view input, Function<Set>& function) {
  return internal::DeserializeImpl(input, function,
                                   static_cast<void*>(nullptr));
}

// TODO: Valdiate that `state` satisfies the requirements with respect to `Set`.
template <int&..., InstructionSet Set, typename SerializationState>
bool Deserialize(std::string_view input, Function<Set>& function,
                 SerializationState& state) {
  return internal::DeserializeImpl(input, function, &state);
}

}  // namespace jasmin

#endif  // JASMIN_SERIALIZATION_H
