#ifndef JASMIN_CORE_DEBUGGER_H
#define JASMIN_CORE_DEBUGGER_H

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include "absl/container/node_hash_map.h"
#include "jasmin/core/function.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/program_fragment.h"
#include "nth/base/attributes.h"
#include "nth/debug/debug.h"

namespace jasmin {

template <InstructionSetType Set>
struct Debugger {
  explicit Debugger(ProgramFragment<Set> &program NTH_ATTRIBUTE(lifetimebound))
      : program_(program) {}

  void set_function_breakpoint(std::string name,
                               std::function<void()> response);

 private:
  ProgramFragment<Set> &program_;
  absl::node_hash_map<std::string,
                      std::pair<Function<Set>, std::function<void()>>>
      breakpoint_functions_;
};

template <InstructionSetType Set>
Debugger(ProgramFragment<Set> &) -> Debugger<Set>;

namespace internal {

template <InstructionSetType Set>
void DebugImpl(Value *vs_head, size_t vs_remaining, Value const *ip,
               FrameBase *cs, uint64_t cs_remaining) {
  auto &[fn, response] =
      *(ip + 1)->as<std::pair<Function<Set>, std::function<void()>> *>();
  response();
  ip = fn.entry();

  NTH_ATTRIBUTE(tailcall)
  return ip->as<exec_fn_type>()(vs_head, vs_remaining, ip, cs, cs_remaining);
}

}  // namespace internal

template <InstructionSetType Set>
void Debugger<Set>::set_function_breakpoint(std::string name,
                                            std::function<void()> response) {
  auto &f               = program_.function(name);
  auto [iter, inserted] = breakpoint_functions_.try_emplace(
      std::move(name), Function<Set>(f.parameter_count(), f.return_count()),
      std::move(response));
  NTH_REQUIRE((harden), inserted);
  iter->second.first = std::move(f);
  f                  = Function<Set>(0, 0);
  f.raw_append(internal::DebugImpl<Set>);
  f.raw_append(&iter->second);
}

}  // namespace jasmin

#endif  // JASMIN_CORE_DEBUGGER_H
