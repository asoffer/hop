#ifndef JASMIN_COMPILE_COMPILED_FUNCTION_H
#define JASMIN_COMPILE_COMPILED_FUNCTION_H

#include <concepts>
#include <span>
#include <string>

namespace jasmin {
struct CompiledFunction {
  operator std::span<std::byte const>() const;

  size_t size() const { return content_.size(); }

  void write(std::initializer_list<uint8_t> instructions);

  void write_at(size_t offset, std::same_as<uint32_t> auto n) {
    write_at_impl(offset, n);
  }

 private:
  void write_at_impl(size_t offset, uint32_t n);
  std::string content_;
};

}  // namespace jasmin

#endif // JASMIN_COMPILE_COMPILED_FUNCTION_H
