#include "jasmin/core/debugger.h"

#include "jasmin/core/instruction.h"

namespace jasmin {
namespace {

void DebugImpl(Value *vs_head, size_t vs_remaining, Value const *ip,
               internal::FrameBase *cs, uint64_t cs_remaining) {
  auto &[fn, response] =
      *(ip + 1)
           ->as<std::pair<std::unique_ptr<Function<>>, std::function<void()>>
                    *>();
  response();
  ip = fn->entry();

  NTH_ATTRIBUTE(tailcall)
  return ip->as<internal::exec_fn_type>()(vs_head, vs_remaining, ip, cs,
                                          cs_remaining);
}

using DebuggerInstructions = jasmin::MakeInstructionSet<>;

}  // namespace

void Debugger::set_function_breakpoint(std::string name,
                                       std::function<void()> response) {
  auto &f               = program_.function(name);
  auto [iter, inserted] = breakpoint_functions_.try_emplace(
      std::move(name), nullptr, std::move(response));
  NTH_REQUIRE((v.harden), inserted);
  iter->second.first = std::exchange(f, nullptr);
  auto fn            = std::make_unique<Function<DebuggerInstructions>>(0, 0);
  f.raw_append(DebugImpl);
  f.raw_append(&iter->second);
  f = std::move(fn);
}

}  // namespace jasmin
