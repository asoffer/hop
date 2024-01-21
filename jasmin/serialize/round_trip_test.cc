#include "jasmin/core/program.h"
#include "jasmin/instructions/common.h"
#include "jasmin/serialize/deserialize.h"
#include "jasmin/serialize/reader.h"
#include "jasmin/serialize/serialize.h"
#include "jasmin/serialize/string_reader.h"
#include "jasmin/serialize/string_writer.h"
#include "jasmin/serialize/writer.h"
#include "nth/test/test.h"

namespace jasmin {
namespace {

std::vector<std::pair<std::string, Function<>&>> FunctionData() { return {}; }

NTH_TEST("round-trip/integer", auto n) {
  std::string s;
  StringWriter w(FunctionData(), s);
  WriteInteger(w, n);

  decltype(n) m;
  StringReader r(s);
  ReadInteger(r, m);
  NTH_ASSERT(r.size() == 0);
  NTH_EXPECT(m == n);
}

NTH_INVOKE_TEST("round-trip/integer") {
  for (int32_t n : {0, 1, -1, 10, -10, 256, -256, 257, -257, 1'000'000,
                    -1'000'000, std::numeric_limits<int32_t>::max(),
                    std::numeric_limits<int32_t>::lowest()}) {
    co_yield n;
  }

  for (uint32_t n : {0u, 1u, 10u, 256u, 257u, 1'000'000u,
                     std::numeric_limits<uint32_t>::max()}) {
    co_yield n;
  }
}

using Set = MakeInstructionSet<Push<Function<>*>>;

NTH_TEST("round-trip/program/empty") {
  Program<Set> p;

  std::string s;
  StringWriter w(p.functions(), s);
  Serialize(p, w);

  Program<Set> q;
  StringReader r(s);
  NTH_ASSERT(Deserialize(r, q));
  NTH_EXPECT(q.function_count() == p.function_count());
}

NTH_TEST("round-trip/program/functions") {
  Program<Set> p;
  auto& f = p.declare("f", 0, 0);
  f.append<Return>();

  std::string s;
  StringWriter w(p.functions(), s);
  Serialize(p, w);

  std::puts("Serialized program:");
  for (size_t i = 0; i < s.size(); ++i) {
    std::printf(" %0.2x", s[i]);
    if (i % 8 == 7) { std::putchar('\n'); }
  }
  std::putchar('\n');

  Program<Set> q;
  StringReader r(s);
  NTH_ASSERT(Deserialize(r, q));
  NTH_EXPECT(q.function_count() == p.function_count());
  NTH_ASSERT(p.function("f").raw_instructions().size() ==
             q.function("f").raw_instructions().size());
  for (size_t i = 0; i < p.function("f").raw_instructions().size(); ++i) {
    NTH_EXPECT(p.function("f").raw_instructions()[i].raw_value() ==
               q.function("f").raw_instructions()[i].raw_value());
  }
}

NTH_TEST("round-trip/program/recursion") {
  Program<Set> p;
  auto& f = p.declare("f", 0, 0);
  f.append<Push<Function<>*>>(&f);
  f.append<Call>({0, 0});
  f.append<Return>();

  std::string s;
  StringWriter w(p.functions(), s);
  Serialize(p, w);

  std::puts("Serialized program:");
  for (size_t i = 0; i < s.size(); ++i) {
    std::printf(" %0.2x", s[i]);
    if (i % 8 == 7) { std::putchar('\n'); }
  }
  std::putchar('\n');

  Program<Set> q;
  StringReader r(s);
  NTH_ASSERT(Deserialize(r, q));
  NTH_EXPECT(q.function_count() == p.function_count());
  NTH_ASSERT(p.function("f").raw_instructions().size() ==
             q.function("f").raw_instructions().size());
  for (size_t i = 0; i < p.function("f").raw_instructions().size(); ++i) {
    if (p.function("f").raw_instructions()[i].raw_value() !=
        q.function("f").raw_instructions()[i].raw_value()) {
      NTH_EXPECT(p.function("f").raw_instructions()[i].raw_value() ==
                 Value(&p.function("f")).raw_value());
      NTH_EXPECT(q.function("f").raw_instructions()[i].raw_value() ==
                 Value(&q.function("f")).raw_value());
    }
  }
}

}  // namespace
}  // namespace jasmin
