#ifndef JASMIN_CORE_FUNCTION_H
#define JASMIN_CORE_FUNCTION_H

#include "jasmin/core/instruction.h"
#include "jasmin/core/instruction_index.h"
#include "jasmin/core/internal/frame.h"
#include "jasmin/core/internal/function_base.h"
#include "jasmin/core/internal/instruction_traits.h"
#include "nth/container/interval.h"
#include "nth/meta/type.h"

namespace jasmin {

template <typename Set = void>
struct Function;

// A representation of a function agnostic to which `InstructionSet` was used to
// construct it. Functions with specified instruction all derive publicly from
// this struct.
template <>
struct Function<void> : internal::FunctionBase {
 protected:
  explicit Function(uint32_t parameter_count, uint32_t return_count,
                    void (*invoke)(nth::stack<Value> &, Value const *))
      : FunctionBase(parameter_count, return_count, invoke) {}
};

// A representation of a function that ties op-codes to instructions (via an
// `InstructionSet` template parameter).
template <typename Set>
struct Function : Function<> {
  using instruction_set = Set;

  static_assert(InstructionSetType<Set>);

  // Constructs an empty `Function` given a `parameter_count` representing
  // the number of parameters to the function, and a `return_count`
  // representing the number of return values for the function.
  explicit Function(uint32_t parameter_count, uint32_t return_count);

  // Appends an op-code for the given instruction `I` template parameter.
  template <typename I>
  requires(instruction_set::instructions.template contains<nth::type<I>>())  //
      constexpr nth::interval<InstructionIndex> append(auto... vs);

  // Appends an op-code for the given instruction `I` template parameter.
  template <typename I>
  requires(instruction_set::instructions.template contains<nth::type<I>>())  //
      constexpr nth::interval<InstructionIndex> append(
          InstructionSpecification spec, auto... vs);

  // Appends an instruction followed by space for `placeholder_count` values
  // which are left uninitialized. They may be initialized later via calls to
  // `Function<...>::set_value`. Returns the corresponding
  // `nth::interval<InstructionIndex>`.
  template <typename I>
  requires(instruction_set::instructions.template contains<nth::type<I>>())  //
      constexpr nth::interval<InstructionIndex> append_with_placeholders();
};

namespace internal {

template <typename StateType>
void FinishExecution(Value *value_stack_head, size_t vs_left, Value const *ip,
                     FrameBase *call_stack, uint64_t cs_remaining) {
  *(ip + 1)->as<Value **>() = value_stack_head;
  *(ip + 2)->as<size_t *>() = vs_left;
  nth::stack<Frame<StateType>>::reconstitute_from(
      static_cast<Frame<StateType> *>(call_stack), cs_remaining);
}

template <typename Set>
constexpr void Invoke(nth::stack<Value> &value_stack, Value const *ip) {
  using frame_type = Frame<FunctionState<Set>>;
  nth::stack<frame_type> call_stack;
  call_stack.emplace();

  auto [top, remaining] = std::move(value_stack).release();
  Value landing_pad[5]  = {Value::Uninitialized(), Value::Uninitialized(),
                           &FinishExecution<FunctionState<Set>>, &top,
                           &remaining};
  call_stack.top().ip   = &landing_pad[0];

  auto [cs_top, cs_remaining] = std::move(call_stack).release();
  ip->as<exec_fn_type>()(top, remaining, ip, cs_top, cs_remaining);
  value_stack = nth::stack<Value>::reconstitute_from(top, remaining);
}

}  // namespace internal

template <typename Set>
Function<Set>::Function(uint32_t parameter_count, uint32_t return_count)
    : Function<>(parameter_count, return_count,
                 internal::Invoke<instruction_set>) {}

template <typename Set>
template <typename I>
requires(Set::instructions.template contains<nth::type<I>>())  //
    constexpr nth::interval<InstructionIndex> Function<Set>::append(
        auto... vs) {
  constexpr size_t DropCount = internal::HasFunctionState<I> ? 2 : 1;
  return internal::InstructionFunctionType<I>()
      .parameters()
      .template drop<DropCount>()
      .reduce([&](auto... ts) {
        return internal::FunctionBase::append(
            {&I::template ExecuteImpl<Set>,
             Value(static_cast<nth::type_t<ts>>(vs))...});
      });
}

template <typename Set>
template <typename I>
requires(Set::instructions.template contains<nth::type<I>>())  //
    constexpr nth::interval<InstructionIndex> Function<Set>::append(
        InstructionSpecification spec, auto... vs) {
  constexpr size_t DropCount = internal::HasFunctionState<I> ? 3 : 2;
  return internal::InstructionFunctionType<I>()
      .parameters()
      .template drop<DropCount>()
      .reduce([&](auto... ts) {
        return internal::FunctionBase::append(
            {&I::template ExecuteImpl<Set>, spec,
             Value(static_cast<nth::type_t<ts>>(vs))...});
      });
}

template <typename Set>
template <typename I>
requires(Set::instructions.template contains<nth::type<I>>())  //
    constexpr nth::interval<InstructionIndex> Function<
        Set>::append_with_placeholders() {
  return internal::FunctionBase::append(&I::template ExecuteImpl<Set>,
                                        ImmediateValueCount<I>());
}

}  // namespace jasmin

#endif  // JASMIN_CORE_FUNCTION_H
