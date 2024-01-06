#ifndef JASMIN_SSA_OP_CODE_H
#define JASMIN_SSA_OP_CODE_H

#include <cstddef>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "jasmin/core/instruction.h"
#include "nth/debug/debug.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {

struct OpCodeMetadata {
  std::string name;
  size_t immediate_value_count;
  size_t parameter_count;
  size_t return_count;
  bool consumes_input;
};

template <InstructionType I>
OpCodeMetadata OpCodeMetadataFor() {
  return {
      .name                  = InstructionName<I>(),
      .immediate_value_count = ImmediateValueCount<I>(),
      .parameter_count       = ParameterCount<I>(),
      .return_count          = ReturnCount<I>(),
      .consumes_input        = ConsumesInput<I>(),
  };
}

namespace internal {

template <InstructionSetType S>
absl::flat_hash_map<exec_fn_type, OpCodeMetadata> MetadataMap =
    S::instructions.reduce([](auto... ts) {
      return absl::flat_hash_map<exec_fn_type, OpCodeMetadata>{
          {&nth::type_t<ts>::template ExecuteImpl<S>,
           OpCodeMetadataFor<nth::type_t<ts>>()}...};
    });

}  // namespace internal

template <InstructionSetType S>
OpCodeMetadata DecodeOpCode(Value v) {
  auto iter = internal::MetadataMap<S>.find(v.as<internal::exec_fn_type>());
  NTH_REQUIRE((v.when(internal::debug)),
              iter != internal::MetadataMap<S>.end());
  return iter->second;
}

}  // namespace jasmin

#endif  // JASMIN_SSA_OP_CODE_H
