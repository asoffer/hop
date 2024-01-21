#include "jasmin/core/instruction.h"
#include "jasmin/instructions/arithmetic.h"
#include "jasmin/instructions/common.h"
#include "jasmin/instructions/compare.h"
#include "jasmin/ssa/register_coalescer.h"
#include "jasmin/ssa/ssa.h"
#include "nth/container/interval.h"
#include "nth/debug/debug.h"
#include "nth/debug/log/stderr_log_sink.h"

auto FibonacciRecursive() {
  using Instructions = jasmin::MakeInstructionSet<
      jasmin::Duplicate, jasmin::Swap, jasmin::Push<uint64_t>,
      jasmin::Push<jasmin::Function<>*>, jasmin::LessThan<uint64_t>,
      jasmin::Add<uint64_t>, jasmin::Subtract<uint64_t>>;
  jasmin::Function<Instructions> func(1, 1);
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push<uint64_t>>(2);
  func.append<jasmin::LessThan<uint64_t>>();
  nth::interval<jasmin::InstructionIndex> jump =
      func.append_with_placeholders<jasmin::JumpIf>();
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push<uint64_t>>(1);
  func.append<jasmin::Subtract<uint64_t>>();
  func.append<jasmin::Push<jasmin::Function<>*>>(&func);
  func.append<jasmin::Call>(
      jasmin::InstructionSpecification{.parameters = 1, .returns = 1});
  func.append<jasmin::Swap>();
  func.append<jasmin::Push<uint64_t>>(2);
  func.append<jasmin::Subtract<uint64_t>>();
  func.append<jasmin::Push<jasmin::Function<>*>>(&func);
  func.append<jasmin::Call>(
      jasmin::InstructionSpecification{.parameters = 1, .returns = 1});
  func.append<jasmin::Add<uint64_t>>();
  nth::interval<jasmin::InstructionIndex> ret = func.append<jasmin::Return>();
  func.set_value(jump, 0, ret.lower_bound() - jump.lower_bound());
  return func;
}

int main() {
  nth::RegisterLogSink(nth::stderr_log_sink);

  jasmin::SsaFunction f(FibonacciRecursive());
  NTH_LOG("Function before optimization:\n{}") <<= {f};

  jasmin::RegisterCoalescer rc;
  rc.Coalesce<typename decltype(FibonacciRecursive())::instruction_set>(f);

  NTH_LOG("Function after optimization:\n{}") <<= {f};

  return 0;
}
