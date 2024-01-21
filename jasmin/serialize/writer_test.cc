#include "jasmin/serialize/writer.h"

#include <span>
#include <string>

#include "jasmin/serialize/string_writer.h"
#include "nth/test/test.h"

namespace jasmin {
namespace {

std::vector<std::pair<std::string, Function<> &>> FunctionData() { return {}; }

NTH_TEST("writer/fixed/char") {
  std::string s;
  StringWriter w(FunctionData(), s);
  WriteFixed(w, 'x');
  NTH_EXPECT(s == "x");
}

NTH_TEST("writer/fixed/uint64_t") {
  std::string s;
  StringWriter w(FunctionData(), s);
  WriteFixed(w, uint64_t{1});
  NTH_ASSERT(s == std::string_view("\x01\x00\x00\x00\x00\x00\x00\x00", 8));

  WriteFixed(w, uint64_t{255});
  NTH_ASSERT(s == std::string_view("\x01\x00\x00\x00\x00\x00\x00\x00"
                                   "\xff\x00\x00\x00\x00\x00\x00\x00",
                                   16));

  WriteFixed(w, uint64_t{257});
  NTH_ASSERT(s == std::string_view("\x01\x00\x00\x00\x00\x00\x00\x00"
                                   "\xff\x00\x00\x00\x00\x00\x00\x00"
                                   "\x01\x01\x00\x00\x00\x00\x00\x00",
                                   24));
  WriteFixed(w, uint32_t{257});
  WriteFixed(w, uint32_t{257});
  NTH_ASSERT(s == std::string_view("\x01\x00\x00\x00\x00\x00\x00\x00"
                                   "\xff\x00\x00\x00\x00\x00\x00\x00"
                                   "\x01\x01\x00\x00\x00\x00\x00\x00"
                                   "\x01\x01\x00\x00\x01\x01\x00\x00",
                                   32));
}

NTH_TEST("writer/fixed/int64_t") {
  std::string s;
  StringWriter w(FunctionData(), s);
  WriteFixed(w, int64_t{1});
  NTH_ASSERT(s == std::string_view("\x01\x00\x00\x00\x00\x00\x00\x00", 8));

  WriteFixed(w, int64_t{255});
  NTH_ASSERT(s == std::string_view("\x01\x00\x00\x00\x00\x00\x00\x00"
                                   "\xff\x00\x00\x00\x00\x00\x00\x00",
                                   16));

  WriteFixed(w, int64_t{257});
  NTH_ASSERT(s == std::string_view("\x01\x00\x00\x00\x00\x00\x00\x00"
                                   "\xff\x00\x00\x00\x00\x00\x00\x00"
                                   "\x01\x01\x00\x00\x00\x00\x00\x00",
                                   24));
  WriteFixed(w, int32_t{-257});
  WriteFixed(w, int32_t{-257});
  NTH_ASSERT(s == std::string_view("\x01\x00\x00\x00\x00\x00\x00\x00"
                                   "\xff\x00\x00\x00\x00\x00\x00\x00"
                                   "\x01\x01\x00\x00\x00\x00\x00\x00"
                                   "\xff\xfe\xff\xff\xff\xfe\xff\xff",
                                   32));
}

NTH_TEST("writer/integer") {
  std::string s;
  StringWriter w(FunctionData(), s);
  WriteInteger(w, 2u);
  NTH_ASSERT(s == std::string_view("\x01\x02", 2));

  s.clear();
  WriteInteger(w, 0u);
  NTH_ASSERT(s == std::string_view("\x00", 1));

  s.clear();
  WriteInteger(w, 257u);
  NTH_ASSERT(s == std::string_view("\x02\x01\x01", 3));

  s.clear();
  WriteInteger(w, -1);
  NTH_EXPECT(s.size() == 3);
  NTH_ASSERT(s == std::string_view("\x02\x01\x01", 3));

  s.clear();
  WriteInteger(w, -257);
  NTH_EXPECT(s.size() == 4);
  NTH_ASSERT(s == std::string_view("\x03\x01\x01\x01", 4));

  s.clear();
  WriteInteger(w, 257);
  NTH_EXPECT(s.size() == 4);
  NTH_ASSERT(s == std::string_view("\x03\x00\x01\x01", 4));
}

}  // namespace
}  // namespace jasmin
