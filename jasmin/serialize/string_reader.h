#ifndef JASMIN_SERIALIZE_STRING_READER_H
#define JASMIN_SERIALIZE_STRING_READER_H

#include <span>
#include <string_view>

#include "absl/container/flat_hash_map.h"
#include "jasmin/core/function.h"

namespace jasmin {

// Writes data to a string referenced by the writer.
struct StringReader {
  explicit StringReader(std::string_view s) : s_(s) {}

  struct cursor_type {
    friend constexpr bool operator==(cursor_type, cursor_type) = default;
    friend constexpr ptrdiff_t operator-(cursor_type lhs, cursor_type rhs) {
      return rhs.dist_ - lhs.dist_;
    }

   private:
    friend StringReader;
    constexpr cursor_type(ptrdiff_t n) : dist_(n) {}
    ptrdiff_t dist_;
  };

  bool read(cursor_type c, std::span<std::byte>) const;
  bool read(std::span<std::byte> buffer);
  bool read(std::byte& b);

  // If `size() >= n`, returns `true` and skips forward `n` bytes, reducing the
  // size by `n`. Otherwise, returns `false` without modifying the reader.
  bool skip(size_t n);

  // Returns the number of bytes left to be read.
  size_t size() const;

  cursor_type cursor() const;

  void register_function(std::string name, Function<>* f);

  friend bool JasminDeserialize(StringReader& r, Function<>*& f);

 private:
  std::string_view s_;
  absl::flat_hash_map<std::string, Function<>*> by_name_;
};

}  // namespace jasmin

#endif  // JASMIN_SERIALIZE_STRING_READER_H
