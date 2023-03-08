#ifndef JASMIN_DEBUG_H
#define JASMIN_DEBUG_H

#include <string>
#include <string_view>

#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "jasmin/internal/instruction_traits.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"

namespace jasmin {

// Returns a string representing the value stored in `v` in a human-readable
// hexadecimal format.
std::string ShowValue(Value v);

std::string ShowValueStack(ValueStack const& v);

struct DumpValueStack : StackMachineInstruction<DumpValueStack> {
  static std::string_view name() { return "dump-value-stack"; }

  static constexpr void execute(ValueStack& value_stack,
                                void (*fn)(std::string_view)) {
    fn(ShowValueStack(value_stack));
  }
};

namespace internal {

template <typename I>
concept HasDebug = requires {
  [](I i, auto&&... xs) { i.debug(xs...); };
};

}  // namespace internal

template <Instruction I>
std::string DumpInstruction(
    std::span<Value const, internal::ImmediateValueCount<I>()> immediates) {
  if constexpr (internal::HasDebug<I>) {
    if constexpr (requires {
                    { I::debug() } -> std::convertible_to<std::string_view>;
                  }) {
      return I::debug();
    } else if constexpr (requires {
                           {
                             I::debug(immediates)
                             } -> std::convertible_to<std::string_view>;
                         }) {
      if constexpr (std::convertible_to<decltype(I::debug(immediates)),
                                        std::string>) {
        return I::debug(immediates);
      } else {
        return std::string(static_cast<std::string_view>(I::debug(immediates)));
      }
    } else {
      static_assert(nth::type<I>.dependent(false),
                    "member function `debug` is not callable as a static "
                    "member function with the appropriate argument.");
    }
  } else {
    std::string result(typeid(I).name());

    for (Value v : immediates) {
      result.append(" ");
      result.append(ShowValue(v));
    }

    return result;
  }
}

namespace internal {

template <typename SetSelfType, Instruction I>
auto MakePair() {
  return std::pair(
      &I::template ExecuteImpl<SetSelfType>,
      +[](std::span<Value const>::iterator& iter) {
        static constexpr size_t Count = ImmediateValueCount<I>();
        ++iter;
        std::span<Value const, Count> span(iter, iter + Count);
        std::string result = DumpInstruction<I>(span);
        iter += Count;
        return result;
      });
}

template <InstructionSet Set>
inline std::array DebugInstructionTable = [] {
  std::array a = Set::instructions.reduce([](auto... ts) {
    return std::array{MakePair<typename Set::self_type, nth::type_t<ts>>()...};
  });

  std::sort(a.begin(), a.end(), [](auto const& lhs, auto const& rhs) {
    return reinterpret_cast<uintptr_t>(lhs.first) <
           reinterpret_cast<uintptr_t>(rhs.first);
  });
  return a;
}();

}  // namespace internal

template <int&..., InstructionSet Set>
std::string DumpFunction(Function<Set> const&f) {
  using instruction_type =
      decltype(&Call::template ExecuteImpl<typename Set::self_type>);

  std::string result;

  std::span<Value const> values = f.raw_instructions();
  auto value_iter               = values.begin();
  while (value_iter != values.end()) {
    auto debug_iter = std::lower_bound(
        internal::DebugInstructionTable<Set>.begin(),
        internal::DebugInstructionTable<Set>.end(),
        value_iter->as<instruction_type>(), [](auto const& pair, auto i) {
          return reinterpret_cast<uintptr_t>(pair.first) <
                 reinterpret_cast<uintptr_t>(i);
        });
    JASMIN_INTERNAL_DEBUG_ASSERT(
        debug_iter != internal::DebugInstructionTable<Set>.end(),
        "Failed to find instruction.");
    result.append(debug_iter->second(value_iter));
    result.append("\n");
  }
  return result;
}

}  // namespace jasmin

#endif  // JASMIN_DEBUG_H
