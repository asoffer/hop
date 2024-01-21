#ifndef JASMIN_SERIALIZE_READER_H
#define JASMIN_SERIALIZE_READER_H

#include <concepts>
#include <cstdint>
#include <optional>
#include <span>
#include <string>

#include "jasmin/core/function.h"

namespace jasmin {

// Concept defining a `Reader`, representing an object from which one can
// extract data to deserialize a `Program`.
template <typename R>
concept Reader = requires(R r, R const& c) {
  typename R::cursor_type;
  {
    c.read(std::declval<typename R::cursor_type>(),
           std::declval<std::span<std::byte>>())
    } -> std::same_as<bool>;
  { r.read(std::declval<std::span<std::byte>>()) } -> std::same_as<bool>;
  { r.skip(size_t{}) } -> std::same_as<bool>;
  { c.size() } -> std::same_as<size_t>;
  {
    r.register_function(std::declval<std::string>(),
                        std::declval<Function<>*>())
    } -> std::same_as<void>;
  { c.cursor() } -> std::same_as<typename R::cursor_type>;
};

// Reads `sizeof(n)` bytes from `r` interpreting them as a `T`. On success,
// returns `true` and populates `x` with the read data. On failure, returns
// `false`, and the value of `x` is unspecified.
template <typename T>
requires std::integral<T> or std::floating_point<T>
bool ReadFixed(Reader auto& r, T& x);

template <int&..., std::unsigned_integral LengthType, Reader R>
std::optional<typename R::cursor_type> ReadLengthPrefix(R& r, LengthType& l);

// Reads a length-prefixed integer from `r` into `n`. The length prefix must fit
// in a single byte, meaning that integers represented must be representable in
// `256` or fewer bytes.
template <int&..., std::unsigned_integral Num>
bool ReadInteger(Reader auto& r, Num& n);
template <int&..., std::signed_integral Num>
bool ReadInteger(Reader auto& r, Num& n);

// Implementation

template <typename T>
requires std::integral<T> or std::floating_point<T>
bool ReadFixed(Reader auto& r, T& x) {
  return r.read(
      std::span<std::byte>(reinterpret_cast<std::byte*>(&x), sizeof(T)));
}

template <int&..., std::unsigned_integral LengthType, Reader R>
std::optional<typename R::cursor_type> ReadLengthPrefix(R& r, LengthType& l) {
  if (not jasmin::ReadFixed(r, l)) { return std::nullopt; }
  auto c = r.cursor();
  if (l > r.size()) { return std::nullopt; }
  return c;
}

template <int&..., std::unsigned_integral Num>
bool ReadInteger(Reader auto& r, Num& n) {
  uint8_t length;
  auto c = jasmin::ReadLengthPrefix(r, length);
  if (not c) { return false; }
  if (sizeof(n) < length) { return false; }
  n = 0;

  for (size_t i = 0; i < length; ++i) {
    std::byte b;
    r.read(b);
    n |= static_cast<Num>(static_cast<uint8_t>(b)) << (CHAR_BIT * i);
  }

  return r.cursor() - *c == length;
}

template <int&..., std::signed_integral Num>
bool ReadInteger(Reader auto& r, Num& n) {
  uint8_t length;
  auto c = jasmin::ReadLengthPrefix(r, length);
  if (not c) { return false; }
  if (sizeof(n) + 1 < length) { return false; }

  std::make_unsigned_t<Num> m = 0;
  std::byte sign;
  r.read(sign);

  for (size_t i = 0; i < length - 1; ++i) {
    std::byte b;
    r.read(b);
    m |= static_cast<decltype(m)>(static_cast<uint8_t>(b)) << (CHAR_BIT * i);
  }
  if (sign == std::byte{1}) { m = ~m + 1; }
  std::memcpy(&n, &m, sizeof(n));

  return r.cursor() - *c == length;
}

}  // namespace jasmin

#endif  // JASMIN_SERIALIZE_READER_H
