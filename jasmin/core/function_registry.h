#ifndef JASMIN_CORE_FUNCTION_REGISTRY_H
#define JASMIN_CORE_FUNCTION_REGISTRY_H

#include <cstddef>
#include <cstdint>

#include "jasmin/core/function_identifier.h"
#include "jasmin/core/internal/function_forward.h"
#include "nth/base/attributes.h"
#include "nth/container/flyweight_map.h"
#include "nth/container/flyweight_set.h"

namespace jasmin {

// A `FunctionRegistry` holds a mapping between pointers to `Function<>` and
// `FunctionId`s. It is the responsibility of a user to ensure that any
// `Function<>` pointer they wish to serialize has been preivously registered
// with the `FunctionRegistry`, and that the `FunctionRegistry` has been
// populated before deserialization of the corresponding `FunctionId`. To ensure
// this property, it is sufficient to ensure that all serialized functions are
// managed by a `ProgramFragment`, and that there is no cycle of functions
// calling through distinct `ProgramFragment`s. Because the serializers for
// `ProgramFragment`s first register all managed functions before serializing
// their implementations, calls within a `ProgramFragment` may have cycles. All
// calls outside a `ProgramFragment` must have already been populated in the
// `FunctionRegistry`, which is guaranteed by the acyclicity condition.
struct FunctionRegistry {
  void register_function(internal::ProgramFragmentBase const &pf
                             NTH_ATTRIBUTE(lifetimebound),
                         Function<> const &f NTH_ATTRIBUTE(lifetimebound));
  FunctionIdentifier get(Function<> const *f);
  Function<> const *operator[](FunctionIdentifier id) const;

 private:
  nth::flyweight_set<internal::ProgramFragmentBase const *> fragments_;
  nth::flyweight_map<Function<> const *, uint32_t> registered_functions_;
};

}  // namespace jasmin

#endif  // JASMIN_CORE_FUNCTION_REGISTRY_H
