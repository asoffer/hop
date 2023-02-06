#include "jasmin/debug.h"

#include <array>
#include <concepts>
#include <cstdint>
#include <string_view>
#include <utility>

#include "jasmin/internal/debug.h"

namespace jasmin {

#if defined(JASMIN_DEBUG)
internal::TypeId DebugTypeId(Value v) { return v.debug_type_id_; }
#endif  // defined(JASMIN_DEBUG)

namespace {

void AppendValue(std::string& out, Value v) {
  static constexpr char HexLookup[17] = "0123456789abcdef";

#if defined(JASMIN_DEBUG)
  auto id = DebugTypeId(v);
  if (id == internal::type_id<bool>) {
    out.append(v.as<bool>() ? "true" : "false");
  } else if (id == internal::type_id<void*>) {
    out.append("addr(0x");
    uint8_t data[ValueSize];
    Value::Store(v, data, ValueSize);
    for (uint8_t const* ptr = data + ValueSize; ptr != data; --ptr) {
      uint8_t c = *(ptr - 1);
      out.push_back(HexLookup[c >> 4]);
      out.push_back(HexLookup[c & 0x0f]);
    }
    out.append(")");
  } else if (id == internal::type_id<int8_t>) {
    out.append(std::to_string(v.as<int8_t>()));
    out.append(" (int8_t)");
  } else if (id == internal::type_id<int16_t>) {
    out.append(std::to_string(v.as<int16_t>()));
    out.append(" (int16_t)");
  } else if (id == internal::type_id<int32_t>) {
    out.append(std::to_string(v.as<int32_t>()));
    out.append(" (int32_t)");
  } else if (id == internal::type_id<int64_t>) {
    out.append(std::to_string(v.as<int64_t>()));
    out.append(" (int64_t)");
  } else if (id == internal::type_id<uint8_t>) {
    out.append(std::to_string(v.as<uint8_t>()));
    out.append(" (uint8_t)");
  } else if (id == internal::type_id<uint16_t>) {
    out.append(std::to_string(v.as<uint16_t>()));
    out.append(" (uint16_t)");
  } else if (id == internal::type_id<uint32_t>) {
    out.append(std::to_string(v.as<uint32_t>()));
    out.append(" (uint32_t)");
  } else if (id == internal::type_id<uint64_t>) {
    out.append(std::to_string(v.as<uint64_t>()));
    out.append(" (uint64_t)");
  } else if (id == internal::type_id<float>) {
    out.append(std::to_string(v.as<float>()));
    out.append(" (float)");
  } else if (id == internal::type_id<double>) {
    out.append(std::to_string(v.as<double>()));
    out.append(" (double)");
  } else {
    uint8_t data[ValueSize];
    Value::Store(v, data, ValueSize);

    std::string_view separator = "";
    for (uint8_t c : data) {
      out.append(std::exchange(separator, " "));
      out.push_back(HexLookup[c >> 4]);
      out.push_back(HexLookup[c & 0x0f]);
    }
  }

#else
  uint8_t data[ValueSize];
  Value::Store(v, data, ValueSize);

  std::string_view separator = "";
  for (uint8_t c : data) {
    out.append(std::exchange(separator, " "));
    out.push_back(HexLookup[c >> 4]);
    out.push_back(HexLookup[c & 0x0f]);
  }
#endif  // defined(JASMIN_DEBUG)
}

}  // namespace

std::string ShowValue(Value v) {
  std::string result;
  AppendValue(result, v);
  return result;
}

std::string ShowValueStack(ValueStack const & v) {
  std::string result;
  if (v.empty()) { return ""; }
  std::string_view separator = "[";
  for (auto* iter = v.end() - v.size(); iter != v.end(); ++iter) {
    result.append(std::exchange(separator, "]\n["));
    AppendValue(result, *iter);
  }
  result.append("]\n");

  return result;
}

}  // namespace jasmin
