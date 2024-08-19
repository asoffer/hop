#include "hop/core/function_registry.h"

namespace hop {

void FunctionRegistry::register_function(
    internal::ProgramFragmentBase const &pf NTH_ATTRIBUTE(lifetimebound),
    Function<> const &f NTH_ATTRIBUTE(lifetimebound)) {
  auto [iter, inserted] = fragments_.insert(&pf);
  registered_functions_.try_emplace(&f, fragments_.index(iter));
}

FunctionIdentifier FunctionRegistry::get(Function<> const *f) {
  auto iter = registered_functions_.find(f);
  if (iter == registered_functions_.end()) {
    return FunctionIdentifier::Invalid();
  }
  return FunctionIdentifier(iter->second, registered_functions_.index(iter));
}

Function<> const *FunctionRegistry::operator[](FunctionIdentifier id) const {
  if (id.index_ >= registered_functions_.size()) { return nullptr; }
  auto [f, pf] = registered_functions_.from_index(id.index_);
  return f;
}

}  // namespace hop
