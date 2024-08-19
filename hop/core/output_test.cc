#include "hop/core/output.h"

#include "nth/test/test.h"

namespace hop {
namespace {

static_assert(Output<>::count == 0);
static_assert(Output<int>::count == 1);
static_assert(Output<int, bool>::count == 2);
static_assert(Output<int, int>::count == 2);

NTH_TEST("output/single") {
  Value vs[1];
  vs[0] = 3;

  Output<int> out(vs);

  out.set<0>(4);
  NTH_EXPECT(vs[0].as<int>() == 4);

  out.set(5);
  NTH_EXPECT(vs[0].as<int>() == 5);
}

NTH_TEST("output/multiple") {
  Value vs[2];
  vs[0] = 3;
  vs[1] = false;

  Output<int, bool> out(vs);

  out.set<0>(4);
  out.set<1>(true);
  NTH_EXPECT(vs[0].as<int>() == 4);
  NTH_EXPECT(vs[1].as<bool>());

  out.set(5, false);
  NTH_EXPECT(vs[0].as<int>() == 5);
  NTH_EXPECT(not vs[1].as<bool>());
}

}  // namespace
}  // namespace hop
