#include "jasmin/serialize/string_writer.h"

#include <span>
#include <string>

#include "nth/test/test.h"

namespace jasmin {
namespace {

std::vector<std::pair<std::string, Function<> &>> FunctionData() { return {}; }

NTH_TEST("StringWriter/basic") {
  std::string s;
  StringWriter w(FunctionData(), s);
  NTH_EXPECT(s.empty());
}

NTH_TEST("StringWriter/allocate") {
  std::string s;
  StringWriter w(FunctionData(), s);
  auto c = w.allocate(4);
  NTH_EXPECT(w.cursor() - c == 4);
  NTH_EXPECT(s.size() == 4);
  w.write(c, std::span(reinterpret_cast<std::byte const *>("abcd"), 4));
  NTH_EXPECT(s.size() == 4);
  NTH_EXPECT(w.cursor() - c == 4);
  NTH_EXPECT(s == "abcd");
}

NTH_TEST("StringWriter/write") {
  std::string s;
  StringWriter w(FunctionData(), s);
  w.write(std::span(reinterpret_cast<std::byte const *>("abcd"), 4));
  NTH_EXPECT(s.size() == 4);
  NTH_EXPECT(s == "abcd");
  w.write(std::span(reinterpret_cast<std::byte const *>("efgh"), 4));
  NTH_EXPECT(s == "abcdefgh");
  w.write(std::byte{'i'});
  NTH_EXPECT(s == "abcdefghi");
}

}  // namespace
}  // namespace jasmin
