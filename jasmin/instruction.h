#ifndef JASMIN_INSTRUCTION_H
#define JASMIN_INSTRUCTION_H

#include <concepts>
#include <limits>
#include <stack>
#include <type_traits>

#include "absl/container/flat_hash_map.h"
#include "jasmin/internal/function_base.h"
#include "jasmin/internal/function_state.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"
#include "nth/base/attributes.h"
#include "nth/debug/debug.h"
#include "nth/meta/concepts.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {
namespace internal {

inline constexpr bool Empty(uint64_t cap_and_left) {
  return (cap_and_left >> 32) - (cap_and_left & 0xffffffff) == 2;
}

struct FrameBase {
  Value const *ip;
};

template <typename StateType>
struct Frame : FrameBase {
  StateType state;
};

template <>
struct Frame<void> : FrameBase {};

using exec_fn_type = void (*)(Value *, size_t, Value const *, FrameBase *,
                              uint64_t);

template <size_t N>
void ReallocateCallStack(Value *value_stack_head, size_t capacity,
                         Value const *ip, FrameBase *call_stack,
                         uint64_t cap_and_left) {
  uint32_t size = cap_and_left >> 32;
  auto *ptr     = static_cast<FrameBase *>(operator new(N *(size << 1)));
  auto *old_ptr = call_stack - (size - 1);
  std::memcpy(ptr, old_ptr, N * size);
  operator delete(old_ptr);

  NTH_ATTRIBUTE(tailcall)
  return ip->template as<exec_fn_type>()(value_stack_head, capacity, ip,
                                         ptr + (size - 1),
                                         cap_and_left * 2 | size);
}

void ReallocateValueStack(Value *value_stack_head, size_t, Value const *ip,
                          FrameBase *call_stack, uint64_t cap_and_left) {
  size_t capacity = value_stack_head->as<size_t>();
  size_t bytes    = capacity * sizeof(Value);

  Value *new_ptr = static_cast<Value *>(operator new(2 * bytes));
  std::memcpy(new_ptr, value_stack_head - (capacity - 1), bytes);
  *(new_ptr + (2 * capacity - 1)) = static_cast<size_t>(2 * capacity);

  operator delete(value_stack_head - (capacity - 1));

  NTH_ATTRIBUTE(tailcall)
  return ip->template as<exec_fn_type>()(new_ptr + capacity - 1, capacity, ip,
                                         call_stack, cap_and_left);
}

struct OpCodeMetadata {
  friend bool operator==(OpCodeMetadata const &,
                         OpCodeMetadata const &) = default;

  size_t op_code_value;
  size_t immediate_value_count;
};

template <typename Inst>
concept HasValueStackRef =
    (nth::type<decltype(Inst::execute)>.parameters().head() ==
     nth::type<ValueStackRef>);

// Base class used solely to indicate that any struct inherting from it is an
// instruction set.
struct InstructionSetBase {};

constexpr bool NonReferencesConvertibleToValues(auto Seq) {
  return (not Seq.template any<[](auto t) {
    return std::is_reference_v<nth::type_t<t>>;
  }>() and Seq.template all<[](auto t) {
    return std::convertible_to<nth::type_t<t>, Value>;
  }>());
}

template <typename I, bool HasFuncState>
constexpr bool ValidSignatureWithImmediatesImpl() {
  constexpr auto signature = nth::type<decltype(I::execute)>;
  if (signature.return_type() != nth::type<ValueStackRef>) { return false; }
  if (nth::type<decltype(I::execute)>.parameters().template get<0>() !=
      nth::type<ValueStackRef>) {
    return false;
  }

  if constexpr (HasFuncState) {
    return nth::type<decltype(I::execute)>.parameters().template get<1>() ==
               nth::type<typename I::function_state &> and
           NonReferencesConvertibleToValues(
               nth::type<decltype(I::execute)>.parameters().template drop<2>());
  } else {
    return NonReferencesConvertibleToValues(
        nth::type<decltype(I::execute)>.parameters().template drop<1>());
  }
}

template <typename I, bool HasFuncState>
constexpr bool ValidSignatureWithoutImmediatesImpl() {
  constexpr auto signature   = nth::type<decltype(I::execute)>;
  constexpr auto return_type = signature.return_type();
  if (return_type != nth::type<void> and
      not std::convertible_to<nth::type_t<return_type>, Value>) {
    return false;
  }

  if constexpr (HasFuncState) {
    return nth::type<decltype(I::execute)>.parameters().template get<0>() ==
               nth::type<typename I::function_state &> and
           NonReferencesConvertibleToValues(
               nth::type<decltype(I::execute)>.parameters().template drop<1>());
  } else {
    return NonReferencesConvertibleToValues(
        nth::type<decltype(I::execute)>.parameters());
  }
}

// Implementation detail. A concept capturing that the `execute` static member
// function of the instruction adheres to one of the supported signatures.
template <typename I, auto Signature = nth::type<decltype(I::execute)>,
          bool HasFuncState = HasFunctionState<I>>
concept HasValidSignature =
    ((HasValueStackRef<I> and
      ValidSignatureWithImmediatesImpl<I, HasFuncState>()) or
     (not HasValueStackRef<I> and
      ValidSignatureWithoutImmediatesImpl<I, HasFuncState>()));

}  // namespace internal

// A concept indicating which types constitute instruction sets understandable
// by Jasmin's interpreter.
template <typename T>
concept InstructionSet = std::derived_from<T, internal::InstructionSetBase>;

// Forward declarations for instructions that need special treatement in
// Jasmin's interpreter and are built-in to every instruction set. Definitions
// appear below.
struct Call;
struct Duplicate;
struct DuplicateAt;
struct Jump;
struct JumpIf;
struct Push;
struct Return;

// Every instruction `Inst` executable as part of Jasmin's stack machine
// interpreter must publicly inherit from `StackMachineInstruction<Inst>`.
// Moreover, other than the builtin-instructions forward-declared above, they
// must also have a static member function `execute` that is not part of an
// overload set (so that `&Inst::execute` is well-formed) and this function
// adhere to one of the following:
//
//   (a) Returns accepts and returns a `jasmin::ValueStackRef`, then a mutable
//       reference to `typename I::function_state` (if and only if that type
//       syntactically valid), and then some number of arguments convertible to
//       `Value`. If present the `typename I::function_state&` parameter is a
//       reference to state shared during a function's execution. The arguments
//       are intperpreted as the immediate values for the instruction. The void
//       return type indicates that execution should fall through to the
//       following instruction.
//
//   (b) Accepts a mutable reference to `typename I::function_state` (if and
//       only if that type syntactically valid), and then some number of
//       arguments convertible to `Value`. The function may return `void` or
//       another type convertible to `Value`. If present the `typename
//       I::function_state&` parameter is a reference to state shared during a
//       function's execution. The arguments are to be popped from the value
//       stack, and the returned value, if any, will be pushed onto the value
//       stack. Execution will fall through to the following instruction.
//
template <typename Inst>
struct StackMachineInstruction {
 private:
  static constexpr bool FS = internal::HasFunctionState<Inst>;
  static constexpr bool VS = internal::HasValueStackRef<Inst>;

  template <InstructionSet Set>
  static void ExecuteImpl(Value *value_stack_head, size_t vs_left,
                          Value const *ip, internal::FrameBase *call_stack,
                          uint64_t cap_and_left) {
    using frame_type = internal::Frame<typename internal::FunctionState<Set>>;
    if constexpr (std::is_same_v<Inst, Call>) {
      if ((cap_and_left & 0xffffffff) == 0) [[unlikely]] {
        return internal::ReallocateCallStack<sizeof(frame_type)>(
            value_stack_head, vs_left, ip, call_stack, cap_and_left);
      } else {
        (++call_stack)->ip   = ip;
        Value const *next_ip = (value_stack_head - 1)
                                   ->as<internal::FunctionBase const *>()
                                   ->entry();
        NTH_ATTRIBUTE(tailcall)
        return next_ip->as<internal::exec_fn_type>()(
            value_stack_head - 1, vs_left + 1, next_ip, call_stack,
            cap_and_left - 1);
      }
    } else if constexpr (std::is_same_v<Inst, Duplicate>) {
      if (vs_left == 0) [[unlikely]] {
        NTH_ATTRIBUTE(tailcall)
        return internal::ReallocateValueStack(value_stack_head, vs_left, ip,
                                              call_stack, cap_and_left);
      } else {
        *value_stack_head = *(value_stack_head - 1);
        NTH_ATTRIBUTE(tailcall)
        return (ip + 1)->template as<internal::exec_fn_type>()(
            value_stack_head + 1, vs_left - 1, (ip + 1), call_stack,
            cap_and_left);
      }
    } else if constexpr (std::is_same_v<Inst, DuplicateAt>) {
      if (vs_left == 0) [[unlikely]] {
        NTH_ATTRIBUTE(tailcall)
        return internal::ReallocateValueStack(value_stack_head, vs_left, ip,
                                              call_stack, cap_and_left);
      } else {
        *value_stack_head = *(value_stack_head - (ip + 1)->as<size_t>());
        NTH_ATTRIBUTE(tailcall)
        return (ip + 2)->template as<internal::exec_fn_type>()(
            value_stack_head + 1, vs_left - 1, (ip + 2), call_stack,
            cap_and_left);
      }

    } else if constexpr (std::is_same_v<Inst, Jump>) {
      Value const *next_ip = ip + (ip + 1)->as<ptrdiff_t>();
      NTH_ATTRIBUTE(tailcall)
      return next_ip->as<internal::exec_fn_type>()(
          value_stack_head, vs_left, next_ip, call_stack, cap_and_left);
    } else if constexpr (std::is_same_v<Inst, JumpIf>) {
      if ((value_stack_head - 1)->as<bool>()) {
        Value const *next_ip = ip + (ip + 1)->as<ptrdiff_t>();
        NTH_ATTRIBUTE(tailcall)
        return next_ip->as<internal::exec_fn_type>()(value_stack_head - 1,
                                                     vs_left + 1, next_ip,
                                                     call_stack, cap_and_left);
      } else {
        Value const *next_ip = ip + 2;
        NTH_ATTRIBUTE(tailcall)
        return next_ip->as<internal::exec_fn_type>()(value_stack_head - 1,
                                                     vs_left + 1, next_ip,
                                                     call_stack, cap_and_left);
      }
    } else if constexpr (std::is_same_v<Inst, Push>) {
      if (vs_left == 0) [[unlikely]] {
        NTH_ATTRIBUTE(tailcall)
        return internal::ReallocateValueStack(value_stack_head, vs_left, ip,
                                              call_stack, cap_and_left);
      } else {
        *value_stack_head = *(ip + 1);
        NTH_ATTRIBUTE(tailcall)
        return (ip + 2)->template as<internal::exec_fn_type>()(
            value_stack_head + 1, vs_left - 1, (ip + 2), call_stack,
            cap_and_left);
      }
    } else if constexpr (std::is_same_v<Inst, Return>) {
      Value const *next_ip = (call_stack--)->ip + 1;
      ++cap_and_left;
      if (internal::Empty(cap_and_left)) [[unlikely]] {
        const_cast<Value &>(*call_stack->ip) = vs_left;
        --call_stack;
        const_cast<Value &>(*call_stack->ip) = value_stack_head;
        std::free(call_stack);
        return;
      } else {
        NTH_ATTRIBUTE(tailcall)
        return next_ip->as<internal::exec_fn_type>()(
            value_stack_head, vs_left, next_ip, call_stack, cap_and_left);
      }
    } else {
#define JASMIN_INTERNAL_GET(p, Ns)                                             \
  (p + Ns)->template as<nth::type_t<parameter_types.template get<Ns>()>>()

      constexpr auto parameter_types =
          nth::type<decltype(Inst::execute)>.parameters().template drop<VS + FS>();
      if constexpr (VS) {
        ValueStackRef vsr = [=]<size_t... Ns>(std::index_sequence<Ns...>) {
          if constexpr (FS) {
            return Inst::execute(
                ValueStackRef(value_stack_head, vs_left),
                std::get<typename Inst::function_state>(
                    static_cast<frame_type *>(call_stack)->state),
                JASMIN_INTERNAL_GET(ip + 1, Ns)...);
          } else {
            return Inst::execute(ValueStackRef(value_stack_head, vs_left),
                                 JASMIN_INTERNAL_GET(ip + 1, Ns)...);
          }
        }
        (std::make_index_sequence<parameter_types.size()>{});

        NTH_ATTRIBUTE(tailcall)
        return (ip + parameter_types.size() + 1)
            ->template as<internal::exec_fn_type>()(
                vsr.end(), vsr.space_remaining(),
                (ip + parameter_types.size() + 1), call_stack, cap_and_left);
      } else {
        constexpr bool ReturnsVoid =
            (nth::type<decltype(Inst::execute)>.return_type() ==
             nth::type<void>);

        [&]<size_t... Ns>(std::index_sequence<Ns...>) {
          auto *p = value_stack_head - parameter_types.size();
          if constexpr (ReturnsVoid) {
            if constexpr (FS) {
              Inst::execute(std::get<typename Inst::function_state>(
                                static_cast<frame_type *>(call_stack)->state),
                            JASMIN_INTERNAL_GET(p, Ns)...);
            } else {
              Inst::execute(JASMIN_INTERNAL_GET(p, Ns)...);
            }
          } else {
            if constexpr (FS) {
              *p = Inst::execute(
                  std::get<typename Inst::function_state>(
                      static_cast<frame_type *>(call_stack)->state),
                  JASMIN_INTERNAL_GET(p, Ns)...);
            } else {
              *p = Inst::execute(JASMIN_INTERNAL_GET(p, Ns)...);
            }
          }
        }
        (std::make_index_sequence<parameter_types.size()>{});
#undef JASMIN_INTERNAL_GET

        constexpr uint32_t ReductionAmount =
            parameter_types.size() - not ReturnsVoid;

        NTH_ATTRIBUTE(tailcall)
        return (ip + 1)->as<internal::exec_fn_type>()(
            value_stack_head - ReductionAmount, vs_left + ReductionAmount,
            ip + 1, call_stack, cap_and_left);
      }
    }
  }
};

// Built-in instructions to every instruction-set.
struct Call : StackMachineInstruction<Call> {
  static constexpr std::string_view debug(std::span<Value const, 0>) {
    return "call";
  }
};

struct Duplicate : StackMachineInstruction<Duplicate> {
  static constexpr std::string_view debug(std::span<Value const, 0>) {
    return "duplicate";
  }
};

struct DuplicateAt : StackMachineInstruction<DuplicateAt> {
  static constexpr std::string debug(
      std::span<Value const, 1> immediate_values) {
    return "duplicate at " + std::to_string(immediate_values[0].as<size_t>());
  }
};

struct Jump : StackMachineInstruction<Jump> {
  static std::string debug(std::span<Value const, 1> immediate_values) {
    ptrdiff_t n = immediate_values[0].as<ptrdiff_t>();
    if (n < 0) {
      return "jump -" + std::to_string(-n);
    } else {
      return "jump +" + std::to_string(n);
    }
  }
};

struct JumpIf : StackMachineInstruction<JumpIf> {
  static std::string debug(std::span<Value const, 1> immediate_values) {
    ptrdiff_t n = immediate_values[0].as<ptrdiff_t>();
    if (n < 0) {
      return "jump-if -" + std::to_string(-n);
    } else {
      return "jump-if +" + std::to_string(n);
    }
  }
};

struct Push : StackMachineInstruction<Push> {
  static constexpr std::string debug(
      std::span<Value const, 1> immediate_values) {
    return "push (" + std::to_string(immediate_values[0].raw_value()) + ")";
  }
};

struct Return : StackMachineInstruction<Return> {
  static constexpr std::string_view debug(std::span<Value const, 0>) {
    return "return";
  }
};

// The `Instruction` concept indicates that a type `I` represents an
// instruction which Jasmin is capable of executing. Instructions must either
// be one of the builtin instructions, or publicly inherit from
// `jasmin::StackMachineInstruction<I>` and have a static member function
// `execute` that is not part of an overload set (so that `&Inst::execute` is
// well-formed) and this function adhere to one of the following:
//
//   (a) Accepts and returns  a `jasmin::ValueStackRef`, then a mutable
//       reference to `typename I::function_state` (if and only if that type
//       syntactically valid), and then some number of arguments convertible
//       to `Value`. If present the `typename I::function_state&` parameter is
//       a reference to state shared during a function's execution. The
//       arguments are intperpreted as the immediate values for the
//       instruction. The void return type indicates that execution should
//       fall through to the following instruction.
//
//   (b) Accepts a mutable reference to `typename I::function_state` (if and
//       only if that type is syntactically valid), and then some number of
//       arguments convertible to `Value`. The function may return `void` or
//       another type convertible to `Value`. If present the `typename
//       I::function_state&` parameter is a reference to state shared during a
//       function's execution. The arguments are to be popped from the value
//       stack, and the returned value, if any, will be pushed onto the value
//       stack. Execution will fall through to the following instruction.
//
template <typename I>
concept Instruction =
    (nth::any_of<I, Call, Duplicate, DuplicateAt, Jump, JumpIf, Push, Return> or
     (std::derived_from<I, StackMachineInstruction<I>> and
      internal::HasValidSignature<I>));

namespace internal {

template <Instruction I>
constexpr size_t ImmediateValueCount() {
  if constexpr (nth::any_of<I, Call, Duplicate, Return>) {
    return 0;
  } else if constexpr (nth::any_of<I, DuplicateAt, Jump, JumpIf, Push>) {
    return 1;
  } else {
    size_t immediate_value_count =
        nth::type<decltype(I::execute)>.parameters().size();
    constexpr bool FS = internal::HasFunctionState<I>;
    constexpr bool VS = internal::HasValueStackRef<I>;

    if (not VS) { return 0; }
    --immediate_value_count;  // Ignore the `ValueStackRef` parameter.
    if (FS) { --immediate_value_count; }
    return immediate_value_count;
  }
}

template <typename I>
concept InstructionOrInstructionSet = Instruction<I> or InstructionSet<I>;

// Constructs an InstructionSet type from a list of instructions. Does no
// checking to validate that `Is` do not contain repeats.
template <Instruction... Is>
struct MakeInstructionSet : InstructionSetBase {
  using self_type                    = MakeInstructionSet;
  static constexpr auto instructions = nth::type_sequence<Is...>;

  // Returns the number of instructions in the instruction set.
  static constexpr size_t size() { return sizeof...(Is); }

  // Returns the `internal::OpCodeMetadata` struct corresponding to the
  // op-code as specified in the byte-code.
  static internal::OpCodeMetadata OpCodeMetadata(Value v) {
    auto iter = Metadata.find(v.as<exec_fn_type>());
    NTH_REQUIRE((v.when(internal::harden)), iter != Metadata.end());
    return iter->second;
  }

  // Returns a `uint64_t` indicating the op-code for the given template
  // parameter instruction `I`.
  template <nth::any_of<Is...> I>
  static constexpr uint64_t OpCodeFor() {
    constexpr size_t value = OpCodeForImpl<I>();
    return value;
  }

  // Returns a `uint64_t` indicating the op-code for the given template
  // parameter instruction `I`.
  template <nth::any_of<Is...> I>
  static constexpr internal::OpCodeMetadata OpCodeMetadataFor() {
    return {.op_code_value         = OpCodeFor<I>(),
            .immediate_value_count = internal::ImmediateValueCount<I>()};
  }

  static auto InstructionFunction(uint64_t op_code) {
    NTH_REQUIRE(op_code < sizeof...(Is)).Log<"Out-of-bounds op-code.">();
    return table[op_code];
  }

 private:
  static constexpr exec_fn_type table[sizeof...(Is)] = {
      &Is::template ExecuteImpl<MakeInstructionSet>...};

  static absl::flat_hash_map<exec_fn_type, internal::OpCodeMetadata> const
      Metadata;

  template <nth::any_of<Is...> I>
  static constexpr uint64_t OpCodeForImpl() {
    // Because the fold-expression below unconditionally adds one to `i` on
    // its first evaluation, we start `i` at its maximum value and allow it to
    // wrap around.
    uint64_t i = std::numeric_limits<uint64_t>::max();
    static_cast<void>(((++i, std::is_same_v<I, Is>) or ...));
    return i;
  }
};

// Given a list of `Instruction`s or `InstructionSet`s,
// `FlattenInstructionList` computes the list of all instructions in the list,
// in an InstructionSet in the list transitively.
constexpr auto FlattenInstructionList(nth::Sequence auto unprocessed,
                                      nth::Sequence auto processed) {
  if constexpr (unprocessed.empty()) {
    return processed;
  } else {
    constexpr auto head = unprocessed.head();
    constexpr auto tail = unprocessed.tail();
    if constexpr (processed.reduce(
                      [&](auto... vs) { return ((vs == head) or ...); })) {
      return FlattenInstructionList(tail, processed);
    } else if constexpr (Instruction<nth::type_t<head>>) {
      // TODO: Is this a bug in Clang? `tail` does not work but
      // `decltype(tail){}` does.
      return FlattenInstructionList(decltype(tail){},
                                    processed + nth::sequence<head>);
    } else {
      return FlattenInstructionList(tail + nth::type_t<head>::instructions,
                                    processed);
    }
  }
}

constexpr auto BuiltinInstructionList =
    nth::type_sequence<Call, Duplicate, DuplicateAt, Jump, JumpIf, Push,
                       Return>;

template <Instruction... Is>
absl::flat_hash_map<exec_fn_type, internal::OpCodeMetadata> const
    MakeInstructionSet<Is...>::Metadata = [] {
      absl::flat_hash_map<exec_fn_type, internal::OpCodeMetadata> result;
      (result.emplace(&Is::template ExecuteImpl<self_type>,
                      OpCodeMetadataFor<Is>()),
       ...);
      return result;
    }();
}  // namespace internal

template <internal::InstructionOrInstructionSet... Is>
using MakeInstructionSet = nth::type_t<
    internal::FlattenInstructionList(
        /*unprocessed=*/nth::type_sequence<Is...>,
        /*processed=*/internal::BuiltinInstructionList)
        .reduce([](auto... vs) {
          return nth::type<internal::MakeInstructionSet<nth::type_t<vs>...>>;
        })>;

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTION_H
