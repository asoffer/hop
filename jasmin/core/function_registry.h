#ifndef JASMIN_CORE_FUNCTION_REGISTRY_H
#define JASMIN_CORE_FUNCTION_REGISTRY_H

#include <cstddef>
#include <cstdint>

#include "nth/base/attributes.h"
#include "nth/container/flyweight_set.h"

namespace jasmin {

struct FunctionRegistry {
  void register_function(Function<> const &f NTH_ATTRIBUTE(lifetimebound)) {
    registered_functions_.insert(&const_cast<Function<>&>(f));
  }

  uint32_t get(Function<> const *f) {
    auto iter = registered_functions_.find(const_cast<Function<> *>(f));
    if (iter == registered_functions_.end()) {
      return static_cast<uint32_t>(-1);
    }
    return registered_functions_.index(iter);
  }

  Function<> *operator[](size_t index) const {
    if (index >= registered_functions_.size()) { return nullptr; }
    return registered_functions_.from_index(index);
  }

  size_t size() const { return registered_functions_.size(); }

  auto const &registered_functions() { return registered_functions_; }

 private:
  nth::flyweight_set<Function<> *> registered_functions_;
};

}  // namespace jasmin

#endif  // JASMIN_CORE_FUNCTION_REGISTRY_H
