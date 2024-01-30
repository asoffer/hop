#ifndef JASMIN_JIT_FUNCTION_H
#define JASMIN_JIT_FUNCTION_H

#include <cstddef>
#include <span>
#include <string>
#include <vector>

namespace jasmin {
namespace internal_jit_function {

struct JitFunctionBase {
  ~JitFunctionBase();
  JitFunctionBase()                    = delete;
  JitFunctionBase(JitFunctionBase const &) = delete;
  JitFunctionBase(JitFunctionBase &&f);
  JitFunctionBase &operator=(JitFunctionBase const &) = delete;
  JitFunctionBase &operator=(JitFunctionBase &&f);

  JitFunctionBase(std::span<std::byte const> code);

 protected:
  void *ptr_;
  size_t size_;
};

}  // namespace internal_jit_function

template <typename F>
struct JitFunction;

template <typename R, typename... Args>
struct JitFunction<R(Args...)>
    : private internal_jit_function::JitFunctionBase {
  JitFunction<R(Args...)>(std::span<std::byte const> code)
      : internal_jit_function::JitFunctionBase(code) {}

  R operator()(Args... args) const {
    return reinterpret_cast<R (*)(Args... args)>(ptr_)(args...);
  };
};

}  // namespace jasmin

#endif  // JASMIN_JIT_FUNCTION_H
