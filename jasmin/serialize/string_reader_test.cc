#include "jasmin/serialize/string_reader.h"

#include <string_view>

#include "nth/test/test.h"

namespace jasmin {
namespace {

NTH_TEST("StringReader/basic") {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz";
  StringReader r(input);
  NTH_ASSERT(r.size() == 26);
  input = "";
  NTH_ASSERT(r.size() == 26);
  r = StringReader(input);
  NTH_ASSERT(r.size() == 0);
}

NTH_TEST("StringReader/skip") {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz";
  StringReader r(input);
  NTH_ASSERT(r.skip(2));
  NTH_ASSERT(r.size() == 24);
  NTH_ASSERT(r.skip(2));
  NTH_ASSERT(r.size() == 22);
  NTH_ASSERT(not r.skip(200));
  NTH_ASSERT(r.size() == 22);
  NTH_ASSERT(r.skip(r.size()));
  NTH_ASSERT(r.size() == 0);
}

NTH_TEST("StringReader/cursor-skip") {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz";
  StringReader r(input);
  auto c = r.cursor();
  NTH_ASSERT(r.skip(2));
  NTH_ASSERT(r.cursor() - c == 2);
}

NTH_TEST("StringReader/read-byte") {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz";
  StringReader r(input);
  std::byte b;
  auto c = r.cursor();
  NTH_ASSERT(r.read(b));
  NTH_EXPECT(b == std::byte{'a'});
  NTH_ASSERT(r.cursor() - c == 1);
}

NTH_TEST("StringReader/read-bytes") {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz";
  StringReader r(input);
  std::array<std::byte, 4> bs;
  auto c = r.cursor();
  NTH_ASSERT(r.read(bs));
  NTH_EXPECT(bs == std::array{
                       std::byte{'a'},
                       std::byte{'b'},
                       std::byte{'c'},
                       std::byte{'d'},
                   });
  NTH_ASSERT(r.cursor() - c == 4);
  std::array<std::byte, 40> too_many_bytes;
  NTH_ASSERT(not r.read(too_many_bytes));
  NTH_ASSERT(r.cursor() - c == 4);
}

NTH_TEST("StringReader/read-bytes-at") {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz";
  StringReader r(input);
  std::array<std::byte, 4> bs;
  auto c = r.cursor();
  r.skip(4);
  NTH_ASSERT(r.read(c, bs));
  NTH_EXPECT(bs == std::array{
                       std::byte{'a'},
                       std::byte{'b'},
                       std::byte{'c'},
                       std::byte{'d'},
                   });
  NTH_ASSERT(r.cursor() - c == 4);
  std::array<std::byte, 100> too_many_bytes;
  NTH_EXPECT(not r.read(c, too_many_bytes));
  NTH_ASSERT(r.cursor() - c == 4);
}

}  // namespace
}  // namespace jasmin
