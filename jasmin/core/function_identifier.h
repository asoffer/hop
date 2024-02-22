#ifndef JASMIN_CORE_FUNCTION_IDENTIFIER_H
#define JASMIN_CORE_FUNCTION_IDENTIFIER_H

#include <cstdint>
#include <limits>

#include "jasmin/core/internal/function_forward.h"
#include "nth/io/deserialize/deserialize.h"
#include "nth/io/reader/reader.h"
#include "nth/io/serialize/serialize.h"
#include "nth/io/writer/writer.h"

namespace jasmin {

// An opaque identifier produced by a `FunctionRegistry` to uniquely identify a
// function. The identifier can be serialized as a mechanism for serializing
// pointers to `Function<>`.
struct FunctionIdentifier {
  constexpr FunctionIdentifier() = default;
  static constexpr FunctionIdentifier Invalid() { return FunctionIdentifier(); }

  template <typename S>
  friend nth::io::serializer_result_type<S> NthSerialize(
      S& s, FunctionIdentifier id) {
    using result_type = nth::io::serializer_result_type<S>;
    if (id == Invalid()) { return result_type(false); }
    if (not nth::io::write_integer(s, id.fragment_)) {
      return result_type(false);
    }
    if (not nth::io::write_integer(s, id.index_)) { return result_type(false); }
    return result_type(true);
  }

  template <typename D>
  friend nth::io::deserializer_result_type<D> NthDeserialize(
      D& d, FunctionIdentifier& id) {
    using result_type = nth::io::deserializer_result_type<D>;
    if (not nth::io::read_integer(d, id.fragment_)) {
      return result_type(false);
    }
    if (not nth::io::read_integer(d, id.index_)) { return result_type(false); }
    if (id == Invalid()) { return result_type(false); }
    return result_type(true);
  }

  friend bool operator==(FunctionIdentifier, FunctionIdentifier) = default;
  friend bool operator!=(FunctionIdentifier, FunctionIdentifier) = default;

 private:
  friend FunctionRegistry;
  explicit FunctionIdentifier(uint32_t fragment, uint32_t index);

  uint32_t fragment_ = 0;
  uint32_t index_    = std::numeric_limits<uint32_t>::max();
};

}  // namespace jasmin

#endif // JASMIN_CORE_FUNCTION_IDENTIFIER_H
