#ifndef JASMIN_SERIALIZE_STRING_WRITER_H
#define JASMIN_SERIALIZE_STRING_WRITER_H

#include <span>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "jasmin/core/function.h"

namespace jasmin {

// Writes data to a string referenced by the writer.
struct StringWriter {
  using cursor_type = size_t;

  // Contsructs a string writer which writes all data to the string referenced
  // by `s`.
  explicit StringWriter(auto const& function_range, std::string& s) : s_(s) {
    for (auto const& [name, fn] : function_range) {
      functions_.emplace(&fn, name);
    }
  }

  cursor_type allocate(size_t n);
  cursor_type cursor() const;

  void write(std::span<std::byte const> data);
  void write(std::byte data);

  void write(cursor_type c, std::span<std::byte const> data);

  friend void JasminSerialize(StringWriter& w, Function<> const* f);

 private:
  std::string& s_;
  absl::flat_hash_map<Function<> const*, std::string_view> functions_;
};

}  // namespace jasmin

#endif  // JASMIN_SERIALIZE_STRING_WRITER_H
