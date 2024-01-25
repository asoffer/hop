#include "jasmin/core/input.h"

#include "nth/test/test.h"

namespace jasmin {
namespace {

static_assert(Input<>::count == 0);
static_assert(Input<int>::count == 1);
static_assert(Input<int, bool>::count == 2);
static_assert(Input<int, int>::count == 2);

NTH_TEST("input/single") {
  Value vs[1];
  vs[0] = 3;
  Input<int> input(vs);
  NTH_EXPECT(input.get<0>() == 3);
  auto [n] = input;
  NTH_EXPECT(n == 3);
}

NTH_TEST("input/multiple") {
  Value vs[2];
  vs[0] = 3;
  vs[1] = true;
  Input<int, bool> input(vs);
  NTH_EXPECT(input.get<0>() == 3);
  NTH_EXPECT(input.get<1>());
  auto [n, b] = input;
  NTH_EXPECT(n == 3);
  NTH_EXPECT(b);
}

}  // namespace
}  // namespace jasmin
