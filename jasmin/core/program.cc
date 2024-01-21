#include "jasmin/core/program.h"

#include <string_view>

#include "jasmin/core/function.h"
#include "nth/debug/debug.h"

namespace jasmin {

Function<>& Program::function(std::string_view name) {
  auto iter = functions_.find(name);
  NTH_REQUIRE((v.harden), iter != functions_.end());
  return *iter->second;
}

Function<> const& Program::function(std::string_view name) const {
  auto iter = functions_.find(name);
  NTH_REQUIRE((v.harden), iter != functions_.end());
  return *iter->second;
}

}  // namespace jasmin
