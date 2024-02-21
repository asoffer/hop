#include "jasmin/core/function.h"
#include "jasmin/core/function_registry.h"
#include "jasmin/core/program_fragment.h"
#include "jasmin/instructions/common.h"
#include "nth/io/reader/reader.h"
#include "nth/io/reader/string.h"
#include "nth/io/writer/string.h"
#include "nth/io/writer/writer.h"
#include "nth/test/test.h"

namespace jasmin {
namespace {

struct Serializer : nth::io::string_writer {
  explicit Serializer(std::string &s) : nth::io::string_writer(s) {}

  FunctionRegistry &context(decltype(nth::type<FunctionRegistry>)) {
    return registry_;
  }

 private:
  FunctionRegistry registry_;
};

struct Deserializer : nth::io::string_reader {
  explicit Deserializer(std::string_view s) : nth::io::string_reader(s) {}

  FunctionRegistry &context(decltype(nth::type<FunctionRegistry>)) {
    return registry_;
  }

  friend bool NthDeserialize(Deserializer &d, std::integral auto &x) {
    return nth::io::read_integer(d, x);
  }

  friend bool NthDeserialize(Deserializer &d, std::floating_point auto &x) {
    return nth::io::read_fixed(d, x);
  }

 private:
  FunctionRegistry registry_;
};

NTH_TEST("round-trip/integer", auto n) {
  std::string content;
  Serializer serializer(content);
  NTH_ASSERT(nth::io::write_integer(serializer, n));

  decltype(n) m;
  Deserializer deserializer(content);
  NTH_ASSERT(nth::io::read_integer(deserializer, m));
  NTH_ASSERT(deserializer.size() == 0);
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

NTH_TEST("round-trip/program-fragment/empty") {
  ProgramFragment<Set> p;

  std::string content;
  Serializer serializer(content);
  NTH_ASSERT(nth::io::serialize(serializer, p));

  ProgramFragment<Set> q;
  Deserializer deserializer(content);
  NTH_ASSERT(nth::io::deserialize(deserializer, q));
  NTH_EXPECT(q.function_count() == p.function_count());
}

NTH_TEST("round-trip/program-fragment/functions") {
  ProgramFragment<Set> p;
  auto& f = p.declare("f", 0, 0).function;
  f.append<Return>();

  std::string content;
  Serializer serializer(content);
  NTH_ASSERT(nth::io::serialize(serializer, p));

  ProgramFragment<Set> q;
  Deserializer deserializer(content);
  NTH_ASSERT(nth::io::deserialize(deserializer, q));
  NTH_EXPECT(q.function_count() == p.function_count());
  NTH_ASSERT(p.function("f").raw_instructions().size() ==
             q.function("f").raw_instructions().size());
  for (size_t i = 0; i < p.function("f").raw_instructions().size(); ++i) {
    NTH_EXPECT(p.function("f").raw_instructions()[i].raw_value() ==
               q.function("f").raw_instructions()[i].raw_value());
  }
}

NTH_TEST("round-trip/program-fragment/recursion") {
  ProgramFragment<Set> p;
  auto& f = p.declare("f", 0, 0).function;
  f.append<Push<Function<>*>>(&f);
  f.append<Call>({0, 0});
  f.append<Return>();

  std::string content;
  Serializer serializer(content);
  NTH_ASSERT(nth::io::serialize(serializer, p));

  ProgramFragment<Set> q;
  Deserializer deserializer(content);
  NTH_ASSERT(nth::io::deserialize(deserializer, q));
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
