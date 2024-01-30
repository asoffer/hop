#include "jasmin/jit/function.h"

#include <sys/mman.h>

#include <cstring>

#include "nth/debug/debug.h"

namespace jasmin {
namespace internal_jit_function {

JitFunctionBase::JitFunctionBase(JitFunctionBase&& f)
    : ptr_(std::exchange(f.ptr_, nullptr)), size_(std::exchange(f.size_, 0)) {}

JitFunctionBase::~JitFunctionBase() {
  if (ptr_) { ::munmap(ptr_, size_); }
}

JitFunctionBase& JitFunctionBase::operator=(JitFunctionBase&& f) {
  ptr_  = std::exchange(f.ptr_, nullptr);
  size_ = std::exchange(f.size_, 0);
  return *this;
}

JitFunctionBase::JitFunctionBase(std::span<std::byte const> code) {
  size_ = code.size();
  ptr_  = ::mmap(0, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
                 -1, 0);
  NTH_REQUIRE((v.always), ptr_ != MAP_FAILED);
  std::memcpy(ptr_, code.data(), size_);
  int protect_result = ::mprotect(ptr_, size_, PROT_READ | PROT_EXEC);
  NTH_REQUIRE((v.always), protect_result != -1);
}

}  // namespace internal_jit_function
}  // namespace jasmin
