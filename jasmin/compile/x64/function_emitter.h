#ifndef JASMIN_COMPILE_X64_FUNCTION_EMITTER_H
#define JASMIN_COMPILE_X64_FUNCTION_EMITTER_H

#include <span>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "jasmin/compile/compiled_function.h"
#include "jasmin/compile/x64/location_map.h"
#include "jasmin/core/instruction.h"
#include "jasmin/ssa/ssa.h"
#include "nth/meta/type.h"

namespace jasmin::x64 {

enum class Register : uint8_t {
  rax = 0,
  rcx = 1,
  rdx = 2,
  rbx = 3,
  rsp = 4,
  rbp = 5,
  rsi = 6,
  rdi = 7,
};

struct FunctionEmitter {
  FunctionEmitter(nth::Type auto instruction_set, auto &generator)
      : metadata_(Metadata<nth::type_t<instruction_set>>()),
        generator_(&generator_) {
    using generator_type = std::remove_reference_t<decltype(generator)>;
    nth::type_t<instruction_set>::instructions.reduce([this](auto... ts) {
      generators_ = {Generate<generator_type>(ts)...};
    });
  }

  void emit(SsaFunction const &fn, CompiledFunction &f);

  void write(std::initializer_list<uint8_t> instructions);

  void push(Register reg);
  void pop(Register reg);
  void mov(Register destination, Register source);
  void ret();
  void syscall();

 private:
  template <typename Generator>
  static auto Generate(nth::Type auto t)
      -> void (*)(void *, FunctionEmitter &, LocationMap const &);

  CompiledFunction *fn_ = nullptr;
  std::vector<size_t> block_starts_;
  absl::flat_hash_map<size_t, size_t> block_jumps_;
  InstructionSetMetadata const &metadata_;
  void *generator_;
  std::vector<void (*)(void *, FunctionEmitter &, LocationMap const &)>
      generators_;
};

// Implementation

template <typename Generator>
auto FunctionEmitter::Generate(nth::Type auto t)
    -> void (*)(void *, FunctionEmitter &, LocationMap const &) {
  if constexpr (t == nth::type<Call>) {
    return nullptr;
  } else if constexpr (t == nth::type<Jump>) {
    return nullptr;
  } else if constexpr (t == nth::type<JumpIf>) {
    return nullptr;
  } else if constexpr (t == nth::type<JumpIfNot>) {
    return nullptr;
  } else if constexpr (t == nth::type<Return>) {
    return nullptr;
  } else {
    return +[](void *gen, FunctionEmitter &cg, LocationMap const &map) {
      (*reinterpret_cast<Generator *>(gen))(decltype(t){}, cg, map);
    };
  }
}

}  // namespace jasmin::x64

#endif  // JASMIN_COMPILE_X64_FUNCTION_EMITTER_H
