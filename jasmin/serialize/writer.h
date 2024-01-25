#ifndef JASMIN_SERIALIZE_WRITER_H
#define JASMIN_SERIALIZE_WRITER_H

#include <concepts>
#include <cstdint>
#include <cstring>
#include <span>

namespace jasmin {

// Concept defining a `Writer`, representing a mechanism by which one can
// serialize a `Program`.
template <typename W>
concept Writer = requires(W w) {
  typename W::cursor_type;
  {
    w.allocate(std::declval<size_t>())
    } -> std::same_as<typename W::cursor_type>;
  { w.cursor() } -> std::same_as<typename W::cursor_type>;
  {
    w.write(std::declval<typename W::cursor_type>(),
            std::declval<std::span<std::byte const>>())
    } -> std::same_as<void>;
  { w.write(std::byte{}) } -> std::same_as<void>;
  { w.write(std::declval<std::span<std::byte const>>()) } -> std::same_as<void>;
};

// Writes `n` to `w` with the same bit-representation, taking exactly
// `sizeof(n)` bytes.
void WriteFixed(Writer auto& w, std::integral auto n);
void WriteFixed(Writer auto& w, std::floating_point auto f);

// Writes a number into the `sizeof(LengthType)` bytes starting at `c` which is
// equal to the number of bytes between `w.cursor()` and then end of the
// location being written to. That is, `w.cursor() - c - sizeof(LengthType)`.
// Behavior is undefined if `w.cursor() - c - sizeof(LengthType)` is not
// expressible as an unsigned number in `sizeof(LengthType)` or fewer bytes.
template <std::unsigned_integral LengthType, int&..., Writer W>
void WritePrefixedLength(W& w, typename W::cursor_type c);

// Writes a length-prefixed integer to `w` with the value `n`. The length prefix
// must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes.
void WriteInteger(Writer auto& w, std::integral auto n);

// Implementation

void WriteFixed(Writer auto& w, std::integral auto n) {
  w.write(std::span(reinterpret_cast<std::byte const*>(std::addressof(n)),
                    sizeof(n)));
}
void WriteFixed(Writer auto& w, std::floating_point auto f) {
  w.write(std::span(reinterpret_cast<std::byte const*>(std::addressof(f)),
                    sizeof(f)));
}

template <std::unsigned_integral LengthType, int&..., Writer W>
void WritePrefixedLength(W& w, typename W::cursor_type c) {
  LengthType length = w.cursor() - c - sizeof(LengthType);
  w.write(c, std::span(reinterpret_cast<std::byte const*>(&length),
                       sizeof(length)));
}

void WriteInteger(Writer auto& w, std::integral auto n) {
  auto c = w.allocate(1);

  std::make_unsigned_t<decltype(n)> m;
  if constexpr (std::is_signed_v<decltype(n)>) {
    std::memcpy(&m, &n, sizeof(m));
    w.write(std::byte{n < 0});
    if (n < 0) { m = ~m + 1; }
  } else {
    m = n;
  }

  if constexpr (sizeof(n) == 1) {
    w.write(static_cast<std::byte>(m));
  } else {
    while (m) {
      w.write(static_cast<std::byte>(m & uint8_t{0xff}));
      m >>= 8;
    }
  }
  jasmin::WritePrefixedLength<uint8_t>(w, c);
}

}  // namespace jasmin

#endif  // JASMIN_SERIALIZE_WRITER_H
