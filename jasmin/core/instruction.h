#ifndef JASMIN_CORE_INSTRUCTION_H
#define JASMIN_CORE_INSTRUCTION_H

#include <concepts>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include "jasmin/core/input.h"
#include "jasmin/core/internal/function_base.h"
#include "jasmin/core/internal/function_state.h"
#include "jasmin/core/internal/instruction_traits.h"
#include "jasmin/core/output.h"
#include "jasmin/core/value.h"
#include "nth/base/attributes.h"
#include "nth/base/pack.h"
#include "nth/container/stack.h"
#include "nth/debug/debug.h"
#include "nth/meta/concepts.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {

// Template from which all stack machine instructions must inherit. This
// template is intended to be used with the curiously recurring template
// pattern, so any instruction should be derived from `jasmin::Instruction`
// instantiated with itself bound to the template parameter. That is,
//
// ```
// struct MyInstruction : jasmin::Instruction<MyInstruction> { ... };
// ```
template <typename>
struct Instruction {
  template <typename>
  static void ExecuteImpl(Value *, size_t, Value const *, internal::FrameBase *,
                          uint64_t);
};

// Every instruction `Inst` executable as part of Jasmin's stack machine
// interpreter must publicly inherit from `jasmin::Instruction<Inst>`. Moreover,
// other than the builtin-instructions forward-declared above, they must also
// have a static member function named either `execute` or `consume` (but not
// both) with the following properties. This function must not be part of an
// overload set so that either `&Inst::execute` or `&Inst::consume` is
// well-formed). A function named `execute` can view some of all of the stack,
// its return value(s) get appended to the stack. A function named `consume` can
// view some of the stack. After execution, the portion of the stack that it can
// view is popped and its return value(s) get appended to the stack.
//
// For the function parameters, there are two formats in which they may be
// provided. Intsructions may either have the number of arguments and returns
// statically determinable by the instruction, or may have this value determined
// by immediate values.
//
// INSTRUCTION-DETERMINED SIGNATURES:
//   If `typename Inst::function_state` is well-formed, the first parameter to
//   the function must be a reference to this type.
//
//   The function must then accept a parameter of the form
//   `jasmin::Input<Is...>` followed by a parameter of the form
//   `jasmin::Output<Os...>` representing access to the inputs and outputs of
//   the instruction respectively. These may then be followed by any number of
//   parameters whose types are convertible to `jasmin::Value`. These parameters
//   are the immediate values to the function. Functions named consume will have
//   their arguments popped from the stack after execution, whereas functions
//   named `execute` will keep their arguments. The function must return void.
//
// IMMEDIATE-VALUE-DETERMINED SIGNATURES:
//   If `typename Inst::function_state` is well-formed, the first parameter to
//   the function must be a reference to this type. The function signature must
//   then accept two `std::span<Value>` parameters followed by any number of
//   parameters, each convertible to `jasmin::Value`. The first span represents
//   the viewable arguments on the stack. The second span represents the
//   locations of return values. The following parameters represent immediate
//   values to the instruction. This function must return `void`.
//
//   When appending an instruction, immediate-value-determined signatures must
//   pass also pass as a first argument to `append` (before any other immediate
//   values) a `jasmin::InstructionSpecification`. Indicating how many
//   parameters and return values are present. Jasmin guarantees that the sizes
//   of the spans provided to the corresponding `consume` or `execute` functions
//   will match this specification.
//
template <typename I>
concept InstructionType = (std::derived_from<I, Instruction<I>> and
                           (internal::BuiltinInstruction<I>() or
                            internal::UserDefinedInstruction<I>));

// A concept indicating which types constitute instruction sets understandable
// by Jasmin's interpreter.
template <typename S>
concept InstructionSetType = std::derived_from<S, internal::InstructionSetBase>;

// Returns `nth::type<F>` where `F` is the type representing the function state
// associated with this instruction, or `nth::type<void>` if the instruction
// does not manipulate any function state.
template <typename I>
constexpr nth::Type auto FunctionState();

// Returns `true` if and only if the number of inputs/outputs processed by the
// function is determined by immediate values.
template <typename I>
constexpr bool ImmediateValueDetermined();

// Returns the number of immediate values passed to the instruction `I`.
template <typename I>
constexpr size_t ImmediateValueCount();

// Returns the number of parameters passed from the stack to the instruction
// `I`.
template <typename I>
constexpr size_t ParameterCount();

// Return `true` if the instruction consumes its input (i.e., is defined via
// `consume`) and `false` otherwise (i.e., is defined via `execute`).
template <typename I>
constexpr bool ConsumesInput();

// Returns the number of return values produced by the instruction.
template <typename I>
constexpr size_t ReturnCount();

// Returns a `std::string_view` representing the name of the instruction. This
// name should not be considered or even unique amongst instructions and should
// no be relied upon for anything other than debugging.
template <typename I>
std::string_view InstructionName();

// `Call` is a built-in instruction, available automatically in every
// instruction set. It pops the top value off the stack, interprets it as a
// function pointer, and begins execution at that function's entry point.
struct Call : Instruction<Call> {};

// `Jump` is a built-in instruction, available automatically in every
// instruction set. It accepts a single `std::ptrdiff_t` immediate value,
// increments the instruction pointer by that amount and resumes execution.
struct Jump : Instruction<Jump> {};

// `JumpIf` is a built-in instruction, available automatically in every
// instruction set. It accepts a single `std::ptrdiff_t` immediate value. It
// pops the top value off the stack and interprets it as a `bool`. If that bool
// is false, execution proceeds normally. Otherwise, the instruction pointer is
// incremented by the immediate value and execution resumes.
struct JumpIf : Instruction<JumpIf> {};

// `JumpIfNot` is a built-in instruction, available automatically in every
// instruction set. It accepts a single `std::ptrdiff_t` immediate value. It
// pops the top value off the stack and interprets it as a `bool`. If that bool
// is true, execution proceeds normally. Otherwise, the instruction pointer is
// incremented by the immediate value and execution resumes.
struct JumpIfNot : Instruction<JumpIfNot> {};

// `Return` is a built-in instruction, available automatically in every
// instruction set. It returns control back to the calling function at the
// instruction pointer immediately following the `Call` instruction that invoked
// this function.
struct Return : Instruction<Return> {};

namespace internal {

inline void ReallocateValueStack(Value *value_stack_head, size_t capacity_left,
                                 Value const *ip, FrameBase *call_stack,
                                 uint64_t cap_and_left) {
  {
    // Scope is necessary to ensure destruction of `v` occurs before the
    // tail-call, even though the destruction will be a no-op due to the
    // move+release.
    auto v =
        nth::stack<Value>::reconstitute_from(value_stack_head, capacity_left);
    v.reserve(v.capacity() * 2);
    std::tie(value_stack_head, capacity_left) = std::move(v).release();
  }

  NTH_ATTRIBUTE(tailcall)
  return ip->template as<exec_fn_type>()(value_stack_head, capacity_left, ip,
                                         call_stack, cap_and_left);
}

template <typename FrameType>
inline void ReallocateCallStack(Value *value_stack_head, size_t capacity_left,
                                Value const *ip, FrameBase *cs_head,
                                uint64_t cs_left) {
  {
    // Scope is necessary to ensure destruction of `c` occurs before the
    // tail-call, even though the destruction will be a no-op due to the
    // move+release.
    auto c = nth::stack<FrameType>::reconstitute_from(
        static_cast<FrameType *>(cs_head), cs_left);
    c.reserve(c.size() * 2);
    std::tie(cs_head, cs_left) = std::move(c).release();
  }

  NTH_ATTRIBUTE(tailcall)
  return ip->template as<exec_fn_type>()(value_stack_head, capacity_left, ip,
                                         cs_head, cs_left);
}

// Constructs an InstructionSet type from a list of instructions. Does no
// checking to validate that `Is` do not contain repeats.
template <InstructionType... Is>
struct MakeInstructionSet : InstructionSetBase {
  static constexpr auto instructions = nth::type_sequence<Is...>;
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
    if constexpr (processed.reduce([&](auto... vs) {
                    return NTH_PACK((any), vs == head);
                  })) {
      return FlattenInstructionList(tail, processed);
    } else if constexpr (InstructionType<nth::type_t<head>>) {
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

}  // namespace internal

template <typename... Is>
using MakeInstructionSet = nth::type_t<
    internal::FlattenInstructionList(
        /*unprocessed=*/nth::type_sequence<Is...>,
        /*processed=*/nth::type_sequence<Call, Jump, JumpIf, JumpIfNot, Return>)
        .reduce([](auto... vs) {
          return nth::type<internal::MakeInstructionSet<nth::type_t<vs>...>>;
        })>;

// Implementation details.

template <typename Inst>
template <typename Set>
void Instruction<Inst>::ExecuteImpl(Value *value_stack_head, size_t vs_left,
                                    Value const *ip,
                                    internal::FrameBase *call_stack,
                                    uint64_t cs_left) {
  using frame_type = internal::Frame<typename internal::FunctionState<Set>>;
  constexpr auto inst_type = nth::type<Inst>;
  if constexpr (inst_type == nth::type<Call>) {
    if (cs_left == 0) [[unlikely]] {
      NTH_ATTRIBUTE(tailcall)
      return internal::ReallocateCallStack<frame_type>(
          value_stack_head, vs_left, ip, call_stack, cs_left);
    } else {
      --value_stack_head;
      auto *p    = new (static_cast<frame_type *>(call_stack)) frame_type;
      p->ip      = ip;
      call_stack = p + 1;
      ip = value_stack_head->as<internal::FunctionBase const *>()->entry();
      NTH_ATTRIBUTE(tailcall)
      return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left + 1, ip,
                                              call_stack, cs_left - 1);
    }
  } else if constexpr (inst_type == nth::type<Jump>) {
    ip += (ip + 1)->as<ptrdiff_t>();
    NTH_ATTRIBUTE(tailcall)
    return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left, ip,
                                            call_stack, cs_left);
  } else if constexpr (inst_type == nth::type<JumpIf>) {
    --value_stack_head;
    if (value_stack_head->as<bool>()) {
      ip += (ip + 1)->as<ptrdiff_t>();
      NTH_ATTRIBUTE(tailcall)
      return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left + 1, ip,
                                              call_stack, cs_left);
    } else {
      ip += 2;
      NTH_ATTRIBUTE(tailcall)
      return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left + 1, ip,
                                              call_stack, cs_left);
    }
  } else if constexpr (inst_type == nth::type<JumpIfNot>) {
    --value_stack_head;
    if (not value_stack_head->as<bool>()) {
      ip += (ip + 1)->as<ptrdiff_t>();
      NTH_ATTRIBUTE(tailcall)
      return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left + 1, ip,
                                              call_stack, cs_left);
    } else {
      ip += 2;
      NTH_ATTRIBUTE(tailcall)
      return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left + 1, ip,
                                              call_stack, cs_left);
    }
  } else if constexpr (inst_type == nth::type<Return>) {
    call_stack = static_cast<frame_type *>(call_stack) - 1;
    ip         = call_stack->ip + 2;
    static_cast<frame_type *>(call_stack)->~frame_type();
    NTH_ATTRIBUTE(tailcall)
    return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left, ip,
                                            call_stack, cs_left + 1);
  } else {
    constexpr auto inst      = internal::InstructionFunctionPointer<Inst>();
    constexpr auto inst_type = internal::InstructionFunctionType<Inst>();
    constexpr bool HasFunctionState =
        (FunctionState<Inst>() != nth::type<void>);
    if constexpr (ImmediateValueDetermined<Inst>()) {
      auto [ins, outs] = (ip + 1)->as<InstructionSpecification>();
      // If we consume, we still need a place to move the inputs during
      // execution. If not, we might need extra space to push return values. In
      // either, we use the stack so the allocation check doesn't have to be
      // guarded by `ConsumesInput`.
      if (vs_left < outs) [[unlikely]] {
        NTH_ATTRIBUTE(tailcall)
        return internal::ReallocateValueStack(value_stack_head, vs_left, ip,
                                              call_stack, cs_left);
      }

      Value *input;
      Value *output;
      if constexpr (ConsumesInput<Inst>()) {
        output = value_stack_head - ins;
        input  = value_stack_head - ins + outs;
        std::memmove(input, output, sizeof(Value) * ins);
      } else {
        input  = value_stack_head - ins;
        output = value_stack_head;
      }

#define JASMIN_CORE_INTERNAL_GET(p, Ns)                                        \
  (p + Ns)->template as<nth::type_t<parameter_types.template get<Ns>()>>()

      constexpr auto parameter_types =
          inst_type.parameters().template drop<2 + HasFunctionState>();
      [&]<size_t... Ns>(std::index_sequence<Ns...>) {
        if constexpr (HasFunctionState) {
          auto &fn_state = std::get<typename Inst::function_state>(
              (static_cast<frame_type *>(call_stack) - 1)->state);

          inst(fn_state, std::span(input, ins), std::span(output, outs),
               JASMIN_CORE_INTERNAL_GET((ip + 2), Ns)...);
        } else {
          inst(std::span(input, ins), std::span(output, outs),
               JASMIN_CORE_INTERNAL_GET((ip + 2), Ns)...);
        }
      }
      (std::make_index_sequence<ImmediateValueCount<Inst>() - 1>{});
      // Note: We subtract one above because we do not need to pass the
      // `InstructionSpecification`. We don't want this reflected in the return
      // of `ImmediateValueCount` since it is technically still an immediate
      // value.

      if constexpr (ConsumesInput<Inst>()) {
        value_stack_head -= ins;
        value_stack_head += outs;
        vs_left += ins;
        vs_left -= outs;
      } else {
        value_stack_head += outs;
        vs_left -= outs;
      }
      ip += 1 + ImmediateValueCount<Inst>();
      NTH_ATTRIBUTE(tailcall)
      return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left, ip,
                                              call_stack, cs_left);
    } else {
      using input_type =
          nth::type_t<inst_type.parameters().template get<HasFunctionState>()>;
      using output_type = nth::type_t<
          inst_type.parameters().template get<1 + HasFunctionState>()>;
      constexpr size_t InputCount  = ParameterCount<Inst>();
      constexpr size_t OutputCount = ReturnCount<Inst>();
      if (vs_left + (ConsumesInput<Inst>() ? InputCount : 0) < OutputCount)
          [[unlikely]] {
        NTH_ATTRIBUTE(tailcall)
        return internal::ReallocateValueStack(value_stack_head, vs_left, ip,
                                              call_stack, cs_left);
      }

      constexpr auto parameter_types =
          inst_type.parameters().template drop<2 + HasFunctionState>();

      if constexpr (HasFunctionState) {
        auto &fn_state = std::get<typename Inst::function_state>(
            (static_cast<frame_type *>(call_stack) - 1)->state);

        [&]<size_t... Ns>(std::index_sequence<Ns...>) {
          if constexpr (ConsumesInput<Inst>()) {
            auto *out_start = value_stack_head - InputCount;
            Value input[InputCount];
            std::memcpy(input, value_stack_head - InputCount,
                        sizeof(Value) * InputCount);
            inst(fn_state, input_type(input), output_type(out_start),
                 JASMIN_CORE_INTERNAL_GET((ip + 1), Ns)...);
          } else {
            inst(fn_state, input_type(value_stack_head - InputCount),
                 output_type(value_stack_head),
                 JASMIN_CORE_INTERNAL_GET((ip + 1), Ns)...);
          }
        }
        (std::make_index_sequence<ImmediateValueCount<Inst>()>{});
      } else {
        [&]<size_t... Ns>(std::index_sequence<Ns...>) {
          if constexpr (ConsumesInput<Inst>()) {
            auto *out_start = value_stack_head - InputCount;
            Value input[InputCount];
            std::memcpy(input, value_stack_head - InputCount,
                        sizeof(Value) * InputCount);
            inst(input_type(input), output_type(out_start),
                 JASMIN_CORE_INTERNAL_GET((ip + 1), Ns)...);
          } else {
            inst(input_type(value_stack_head - InputCount),
                 output_type(value_stack_head),
                 JASMIN_CORE_INTERNAL_GET((ip + 1), Ns)...);
          }
        }
        (std::make_index_sequence<ImmediateValueCount<Inst>()>{});
      }
#undef JASMIN_CORE_INTERNAL_GET

      constexpr ptrdiff_t UpdateAmount =
          OutputCount - (ConsumesInput<Inst>() ? InputCount : 0);
      if constexpr (UpdateAmount < 0) {
        value_stack_head -= -UpdateAmount;
      } else {
        value_stack_head += UpdateAmount;
      }
      vs_left -= UpdateAmount;

      ip += ImmediateValueCount<Inst>() + 1;
      NTH_ATTRIBUTE(tailcall)
      return ip->as<internal::exec_fn_type>()(value_stack_head, vs_left, ip,
                                              call_stack, cs_left);
    }
  }
}

template <typename I>
constexpr nth::Type auto FunctionState() {
  if constexpr (requires { typename I::function_state; }) {
    return nth::type<typename I::function_state>;
  } else {
    return nth::type<void>;
  }
}

template <typename I>
constexpr bool ImmediateValueDetermined() {
  return nth::type<I> != nth::type<jasmin::Jump> and
         nth::type<I> != nth::type<jasmin::JumpIf> and
         nth::type<I> != nth::type<jasmin::JumpIfNot> and
         not std::derived_from<
             nth::type_t<
                 internal::InstructionFunctionType<I>()
                     .parameters()
                     .template get<FunctionState<I>() != nth::type<void>>()>,
             internal::InputBase>;
}

template <typename I>
constexpr size_t ImmediateValueCount() {
  if constexpr (nth::any_of<I, Return>) {
    return 0;
  } else if constexpr (nth::any_of<I, Call, Jump, JumpIf, JumpIfNot>) {
    return 1;
  } else {
    return internal::InstructionFunctionType<I>().parameters().size() -
           (FunctionState<I>() == nth::type<void> ? 2 : 3) +
           ImmediateValueDetermined<I>();
  }
}

template <typename I>
constexpr size_t ParameterCount() {
  if constexpr (nth::any_of<I, Jump, Return>) {
    return 0;
  } else if constexpr (nth::any_of<I, Call, JumpIf, JumpIfNot>) {
    return 1;
  } else {
    constexpr auto parameters =
        internal::InstructionFunctionType<I>().parameters();
    if constexpr (ImmediateValueDetermined<I>()) {
      return -1;
    } else {
      return nth::type_t<parameters.template get<FunctionState<I>() !=
                                                 nth::type<void>>()>::count;
    }
  }
}

template <typename I>
constexpr bool ConsumesInput() {
  if constexpr (nth::any_of<I, JumpIf, JumpIfNot, Call>) {
    return true;
  } else {
    return requires { &I::consume; };
  }
}

template <typename I>
constexpr size_t ReturnCount() {
  if constexpr (nth::any_of<I, Jump, Return, JumpIf, JumpIfNot>) {
    return 0;
  } else if constexpr (nth::type<I> == nth::type<Call>) {
    return -1;
  } else {
    constexpr auto parameters =
        internal::InstructionFunctionType<I>().parameters();
    if constexpr (ImmediateValueDetermined<I>()) {
      return -1;
    } else {
      return nth::type_t<parameters.template get<
          1 + (FunctionState<I>() != nth::type<void>)>()>::count;
    }
  }
}

template <typename I>
std::string_view InstructionName() {
  return nth::type<I>.name();
}

}  // namespace jasmin

#endif  // JASMIN_CORE_INSTRUCTION_H
