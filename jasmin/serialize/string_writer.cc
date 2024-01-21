#include "jasmin/serialize/string_writer.h"

#include <cstring>

#include "jasmin/serialize/writer.h"

namespace jasmin {

void StringWriter::write(std::span<std::byte const> data) {
  auto const* p = reinterpret_cast<char const*>(data.data());
  s_.insert(s_.end(), p, p + data.size());
}

void StringWriter::write(std::byte data) {
  s_.push_back(static_cast<char>(data));
}

StringWriter::cursor_type StringWriter::allocate(size_t n) {
  auto c = cursor();
  s_.resize(s_.size() + n);
  return c;
}

StringWriter::cursor_type StringWriter::cursor() const { return s_.size(); }

void StringWriter::write(cursor_type c, std::span<std::byte const> data) {
  std::memcpy(s_.data() + c, data.data(), data.size());
}

void JasminSerialize(StringWriter& w, Function<> const* f) {
  auto iter = w.functions_.find(f);
  NTH_REQUIRE(iter != w.functions_.end());
  std::string_view name = iter->second;
  jasmin::WriteFixed(w, static_cast<uint32_t>(name.size()));
  w.write(std::span<std::byte const>(
      reinterpret_cast<std::byte const*>(name.data()), name.size()));
}

}  // namespace jasmin
