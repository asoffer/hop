#include "jasmin/serialize/string_reader.h"

#include "jasmin/serialize/reader.h"

namespace jasmin {

bool StringReader::read(cursor_type c, std::span<std::byte> buffer) const {
  if (c.dist_ < buffer.size()) { return false; }
  std::memcpy(buffer.data(), s_.end() - c.dist_, buffer.size());
  return true;
}

bool StringReader::read(std::byte& b) {
  if (s_.empty()) { return false; }
  b = static_cast<std::byte>(s_[0]);
  s_.remove_prefix(1);
  return true;
}

bool StringReader::read(std::span<std::byte> buffer) {
  if (s_.size() < buffer.size()) { return false; }
  std::memcpy(buffer.data(), s_.data(), buffer.size());
  s_.remove_prefix(buffer.size());
  return true;
}

bool StringReader::skip(size_t n) {
  if (s_.size() < n) { return false; }
  s_.remove_prefix(n);
  return true;
}

size_t StringReader::size() const { return s_.size(); }

typename StringReader::cursor_type StringReader::cursor() const {
  return cursor_type(s_.size());
}

bool JasminDeserialize(StringReader& r, Function<>*& f) {
  uint32_t length;
  if (not jasmin::ReadFixed(r, length)) { return false; }

  std::string name(length, '\0');
  if (not r.read(std::span<std::byte>(reinterpret_cast<std::byte*>(name.data()),
                                      length))) {
    return false;
  }
  auto iter = r.by_name_.find(name);
  if (iter == r.by_name_.end()) { return false; }
  f = iter->second;
  return true;
}

void StringReader::register_function(std::string name, Function<>* f) {
  by_name_.emplace(std::move(name), f);
}

}  // namespace jasmin
