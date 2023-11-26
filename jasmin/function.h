#ifndef JASMIN_FUNCTION_H
#define JASMIN_FUNCTION_H

#include "jasmin/instruction.h"
#include "jasmin/instruction_index.h"
#include "jasmin/internal/function_base.h"
#include "jasmin/internal/instruction_traits.h"
#include "nth/container/interval.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {
namespace internal {

template <typename I, typename Set>
concept ContainedIn = Set::instructions.template contains<nth::type<I>>();

}  // namespace internal

// A representation of a function that ties op-codes to instructions (via an
// InstructionSet template parameter).
template <InstructionSetType Set>
struct Function : internal::FunctionBase {
 private:
  template <InstructionType I>
  static Value ExecPtr() {
    return &I::template ExecuteImpl<typename Set::self_type>;
  }

 public:
  // Constructs an empty `Function` given a `parameter_count` representing
  // the number of parameters to the function, and a `return_count`
  // representing the number of return values for the function.
  explicit constexpr Function(uint8_t parameter_count, uint8_t return_count)
      : FunctionBase(parameter_count, return_count) {}

  // Appends an op-code for the given instruction `I` template parameter.
  template <internal::ContainedIn<Set> I>
  constexpr nth::interval<InstructionIndex> append(auto... vs) {
    // TODO: Check immediates
    return internal::FunctionBase::append({ExecPtr<I>(), Value(vs)...});
  }

  // Appends an instruction followed by space for `placeholder_count` values
  // which are left uninitialized. They may be initialized later via calls to
  // `Function<...>::set_value`. Returns the corresponding
  // `nth::interval<InstructionIndex>`.
  template <internal::ContainedIn<Set> I>
  constexpr nth::interval<InstructionIndex> append_with_placeholders() {
    return internal::FunctionBase::append(ExecPtr<I>(),
                                          ImmediateValueCount<I>());
  }
};

}  // namespace jasmin

#endif  // JASMIN_FUNCTION_H
