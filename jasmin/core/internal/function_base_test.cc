#include "jasmin/core/internal/function_base.h"

#include "nth/test/test.h"

namespace jasmin::internal {
namespace {

NTH_TEST("function-base/simple") {
  FunctionBase f(1, 2);
  NTH_EXPECT(f.parameter_count() == size_t{1});
  NTH_EXPECT(f.return_count() == size_t{2});
  NTH_EXPECT(f.raw_instructions().empty());
}

NTH_TEST("function-base/raw-append") {
  FunctionBase f(0, 0);
  f.raw_append(3);
  f.raw_append(true);
  NTH_ASSERT(f.raw_instructions().size() == size_t{2});
  NTH_EXPECT(f.raw_instructions()[0].as<int>() == 3);
  NTH_EXPECT(f.raw_instructions()[1].as<bool>());
}

}  // namespace
}  // namespace jasmin::internal
