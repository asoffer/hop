#ifndef JASMIN_SERIALIZATION_H
#define JASMIN_SERIALIZATION_H

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "jasmin/internal/debug.h"
#include "jasmin/internal/type_list.h"

namespace jasmin {
namespace internal {

struct InstructionMapEntry {
  uintptr_t function;
  uint64_t op_code;
  uint64_t immediate_value_count;
};

template <InstructionSet Set>
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
    };
    std::cerr << typeid(I).name() << " imm: " << ImmediateValueCount<I>() << "\n";
  });


  std::sort(result.begin(), result.end(),
            [](InstructionMapEntry const& lhs, InstructionMapEntry const& rhs) {
              return lhs.function < rhs.function;
            });
  return result;
}

template <InstructionSet Set>
auto const& InstructionMap() {
  static auto const result = InstructionMapImpl<Set>();
  return result;
}

// Copies raw instructions to output, returning a pointer to the first byte of
// the first instruction.
char* CopyRawInstructions(std::span<Value const> raw_instructions,
                          std::string& output);

// Replaces the instruction execution function with its op-code, and returns the
// number of immediate values for the instruction.
template <InstructionSet Set>
size_t UpdateOpCode(char* p) {
  using instruction_type = decltype(&Call::template ExecuteImpl<Set>);
  auto const& Instructions = internal::InstructionMap<Set>();

  instruction_type instruction_ptr;
  std::memcpy(&instruction_ptr, p, sizeof(instruction_type));

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
  std::memcpy(p, &iter->op_code, ValueSize);
  return iter->immediate_value_count;
}

}  // namespace internal

// Serializes the instructions in `function` to `output`. The only guarantee
// provided is that if a function is serialized to some string and then that
// string is deserialized (see below) to some function, that the two functions
// will have the same instructions (i.e., with the same op-codes) and immediate
// values (serialized as if by `std::memcpy`).
//
// Note that it is safe to serialize and deserialize in separate binaries, or
// separate invocations of the same binary, provided they were compiled with
// identical versions of Jasmin with the same compilation arguments.
template <int&..., InstructionSet Set>
void Serialize(Function<Set> const& function, std::string& output) {
  char* p = internal::CopyRawInstructions(function.raw_instructions(), output);
  char const* end = output.data() + output.size();
  while (p != end) {
    size_t immediate_value_count = internal::UpdateOpCode<Set>(p);
    p += (1 + immediate_value_count) * ValueSize;
  }
}

// Deserializes `input` into `function`. The string `input` must be a string
// that was serialized (or could have been serialized) by `Serialize` (defined
// above). See caveats in documentation above regarding the validity of
// serialization across binaries.
template <int&..., InstructionSet Set>
void Deserialize(std::string_view input, Function<Set>& function) {
  JASMIN_INTERNAL_DEBUG_ASSERT(
      input.size() % ValueSize == 0,
      "Input must have a length that is a multiple of `jasmin::ValueSize`.");
  if (input.empty()) { return; }

  auto const& Instructions = internal::InstructionMap<Set>();

  char const* p = input.data();
  while (p != input.data() + input.size()) {
    uint64_t op_code = Value::Load(p, ValueSize).as<uint64_t>();

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

    p += ValueSize;
    function.raw_append(instruction_ptr);
    for (size_t i = 0; i < iter->immediate_value_count; ++i) {
      function.raw_append(Value::Load(p, ValueSize));
      p += ValueSize;
    }

  }
}

}  // namespace jasmin

#endif  // JASMIN_SERIALIZATION_H
