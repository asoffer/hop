#ifndef JASMIN_CORE_FUNCTION_H
#define JASMIN_CORE_FUNCTION_H

#include "jasmin/core/function_identifier.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/instruction_index.h"
#include "jasmin/core/internal/frame.h"
#include "jasmin/core/internal/function_base.h"
#include "jasmin/core/internal/function_forward.h"
#include "jasmin/core/internal/instruction_traits.h"
#include "jasmin/core/metadata.h"
#include "nth/container/interval.h"
#include "nth/io/serialize/serialize.h"
#include "nth/meta/type.h"

namespace jasmin {
namespace internal {

template <typename S, InstructionType I>
nth::io::serializer_result_type<std::remove_reference_t<S>>
InstructionSerializer(
    S s, std::span<Value const> v) requires std::is_lvalue_reference_v<S>;

template <typename D, InstructionType I>
nth::io::deserializer_result_type<std::remove_reference_t<D>>
InstructionDeserializer(D d,
                        Function<> &fn) requires std::is_lvalue_reference_v<D>;

}  // namespace internal

// A representation of a function agnostic to which `InstructionSet` was used to
// construct it. Functions with specified instruction all derive publicly from
// this struct.
template <>
struct Function<void> : internal::FunctionBase {
  template <nth::io::serializer_with_context<FunctionRegistry> S>
  friend nth::io::serializer_result_type<S> NthSerialize(S &s,
                                                         Function const *f) {
    if (not f) { NTH_UNIMPLEMENTED(); }
    return nth::io::serialize(s, s.context(nth::type<FunctionRegistry>).get(f));
  }

  template <nth::io::deserializer_with_context<FunctionRegistry> D>
  friend nth::io::deserializer_result_type<D> NthDeserialize(D &d,
                                                             Function *&fn) {
    return nth::io::deserialize(d, const_cast<Function const *&>(fn));
  }

  template <nth::io::deserializer_with_context<FunctionRegistry> D>
  friend nth::io::deserializer_result_type<D> NthDeserialize(
      D &d, Function const *&fn) {
    using result_type = nth::io::deserializer_result_type<D>;
    FunctionIdentifier id;
    result_type result = nth::io::deserialize(d, id);
    if (not result) { return result; }
    auto &registry = d.context(nth::type<FunctionRegistry>);
    fn             = registry[id];
    if (fn == nullptr) { NTH_UNIMPLEMENTED(); }
    return result_type(fn != nullptr);
  }

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

  // Constructs an empty function to be used only for overwriting.
  explicit Function();

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

  template <nth::io::serializer_with_context<FunctionRegistry> S>
  friend nth::io::serializer_result_type<S> NthSerialize(S &s,
                                                         Function const &fn) {
    using result_type = nth::io::serializer_result_type<S>;
    if (not nth::io::write_integer(s, fn.parameter_count())) {
      return result_type(false);
    }
    if (not nth::io::write_integer(s, fn.return_count())) {
      return result_type(false);
    }
    auto const &set_metadata = Metadata<Set>();
    std::optional c          = s.allocate(sizeof(uint32_t));
    if (not c) { return result_type(false); }
    std::span insts = fn.raw_instructions();

    static constexpr auto Serializers =
        Set::instructions.reduce([](auto... ts) {
          return std::array<result_type (*)(decltype(s), std::span<Value const>),
                            sizeof...(ts)>{
              internal::InstructionSerializer<decltype(s), nth::type_t<ts>>...};
        });

    while (not insts.empty()) {
      uint16_t index = set_metadata.opcode(insts[0]);
      if (not nth::io::write_fixed(s, index)) { return result_type(false); }
      auto immediate_value_count =
          set_metadata.metadata(index).immediate_value_count;
      result_type result =
          Serializers[index](s, insts.subspan(1, immediate_value_count));
      if (not result) { return result; }
      insts = insts.subspan(immediate_value_count + 1);
    }
    uint16_t distance = static_cast<uint16_t>(s.cursor() - *c);
    return result_type(s.write_at(*c, nth::bytes(distance)));
  }

  template <nth::io::serializer_with_context<FunctionRegistry> S>
  friend nth::io::serializer_result_type<S> NthSerialize(S &s,
                                                         Function const *f) {
    return nth::io::serialize(s, static_cast<Function<> const *>(f));
  }

  template <nth::io::deserializer_with_context<FunctionRegistry> D>
  friend nth::io::deserializer_result_type<D> NthDeserialize(D &d,
                                                             Function &fn) {
    using result_type = nth::io::deserializer_result_type<D>;
    uint32_t parameter_count, return_count;
    if (not nth::io::read_integer(d, parameter_count)) {
      return result_type(false);
    }
    if (not nth::io::read_integer(d, return_count)) {
      return result_type(false);
    }
    fn = Function(parameter_count, return_count);

    uint32_t expected_length;
    auto c = d.cursor();
    if (not nth::io::read_fixed(d, expected_length)) {
      return result_type(false);
    }

    auto const &set_metadata = Metadata<Set>();
    static constexpr auto Deserializers =
        Set::instructions.reduce([](auto... ts) {
          return std::array<result_type (*)(decltype(d), Function<> &),
                            sizeof...(ts)>{
              internal::InstructionDeserializer<decltype(d),
                                                nth::type_t<ts>>...};
        });

    while (d.cursor() - c < expected_length) {
      uint16_t opcode;
      if (not nth::io::read_fixed(d, opcode)) { return result_type(false); }

      fn.raw_append(set_metadata.function(opcode));
      result_type result = Deserializers[opcode](d, fn);
      if (not result) { return result; }
    }

    return result_type(d.cursor() - c == expected_length);
  }

  template <nth::io::deserializer_with_context<FunctionRegistry> D>
  friend nth::io::deserializer_result_type<D> NthDeserialize(D &d,
                                                             Function *&f) {
    Function<> *fn;
    auto result = nth::io::deserialize(d, fn);
    if (not result) { return result; }
    f = static_cast<Function *>(fn);
    return result;
  }

  template <nth::io::deserializer_with_context<FunctionRegistry> D>
  friend nth::io::deserializer_result_type<D> NthDeserialize(
      D &d, Function const *f) {
    return nth::io::deserialize(d, static_cast<Function<> const *&>(f));
  }
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
Function<Set>::Function()
    : Function<>(0, 0, internal::Invoke<instruction_set>) {}

template <typename Set>
template <typename I>
requires(Set::instructions.template contains<nth::type<I>>())  //
    constexpr nth::interval<InstructionIndex> Function<Set>::append(
        auto... vs) {
  if constexpr (nth::type<I> == nth::type<Return>) {
    return internal::FunctionBase::append({&I::template ExecuteImpl<Set>});
  } else if constexpr (nth::type<I> == nth::type<jasmin::Jump>) {
    static_assert(sizeof...(vs) == 0);
    return internal::FunctionBase::append({&I::template ExecuteImpl<Set>});
  } else if constexpr (nth::type<I> == nth::type<jasmin::JumpIf>) {
    static_assert(sizeof...(vs) == 1);
    return internal::FunctionBase::append(
        {&I::template ExecuteImpl<Set>, static_cast<size_t>(vs)...});
  } else {
    constexpr size_t DropCount = internal::HasFunctionState<I> ? 3 : 2;
    return internal::InstructionFunctionType<I>()
        .parameters()
        .template drop<DropCount>()
        .reduce([&](auto... ts) {
          return internal::FunctionBase::append(
              {&I::template ExecuteImpl<Set>,
               Value(static_cast<nth::type_t<ts>>(vs))...});
        });
  }
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

namespace internal {

template <typename S, InstructionType I>
nth::io::serializer_result_type<std::remove_reference_t<S>>
InstructionSerializer(
    S s, std::span<Value const> v) requires std::is_lvalue_reference_v<S> {
  using result_type =
      nth::io::serializer_result_type<std::remove_reference_t<S>>;
  if constexpr (nth::type<I> == nth::type<Return>) {
    return result_type(true);
  } else if constexpr (nth::type<I> == nth::type<Call>) {
    return nth::io::serialize(s, v[0].as<InstructionSpecification>());
  } else if constexpr (nth::any_of<I, Jump, JumpIf, JumpIfNot>) {
    return result_type(nth::io::write_integer(s, v[0].as<ptrdiff_t>()));
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
          result_type result(true);
          (void)(static_cast<bool>(
                     result = nth::io::serialize(
                         s, v[i++].template as<nth::type_t<ts>>())) and
                 ...);
          return result;
        });
  }
}

template <typename D, InstructionType I>
nth::io::deserializer_result_type<std::remove_reference_t<D>>
InstructionDeserializer(D d,
                        Function<> &fn) requires std::is_lvalue_reference_v<D> {
  using result_type =
      nth::io::deserializer_result_type<std::remove_reference_t<D>>;
  if constexpr (nth::type<I> == nth::type<Return>) {
    return result_type(true);
  } else if constexpr (nth::type<I> == nth::type<Call>) {
    InstructionSpecification spec;
    result_type result = nth::io::deserialize(d, spec);
    if (not result) { return result; }
    fn.raw_append(spec);
    return result_type(true);
  } else if constexpr (nth::type<I> == nth::type<Jump> or
                       nth::type<I> == nth::type<JumpIf>) {
    ptrdiff_t amount;
    if (not nth::io::read_integer(d, amount)) { return result_type(false); }
    fn.raw_append(amount);
    return result_type(true);
  } else {
    constexpr bool HasFunctionState = internal::HasFunctionState<I>;
    constexpr auto parameters =
        internal::InstructionFunctionType<I>().parameters();
    return parameters.template drop<HasFunctionState + 2>().reduce(
        [&](auto... ts) {
          return ([&](auto t) {
            nth::type_t<t> value;
            result_type result = nth::io::deserialize(d, value);
            if (not result) { return result; }
            fn.raw_append(value);
            return result_type(true);
          }(ts) and
                  ...);
        });
  }
}

}  // namespace internal
}  // namespace jasmin

#endif  // JASMIN_CORE_FUNCTION_H
