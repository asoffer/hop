#include <cinttypes>
#include <cstdio>

#include "jasmin/core/instruction.h"
#include "jasmin/instructions/arithmetic.h"
#include "jasmin/instructions/common.h"
#include "jasmin/instructions/compare.h"
#include "jasmin/serialize/deserialize.h"
#include "jasmin/serialize/serialize.h"
#include "jasmin/serialize/string_reader.h"
#include "jasmin/serialize/string_writer.h"
#include "nth/container/interval.h"
#include "nth/debug/debug.h"
#include "nth/debug/log/stderr_log_sink.h"

using Instructions = jasmin::MakeInstructionSet<
    jasmin::Duplicate, jasmin::Swap, jasmin::Push<uint64_t>,
    jasmin::Push<jasmin::Function<>*>, jasmin::LessThan<uint64_t>,
    jasmin::Add<uint64_t>, jasmin::Subtract<uint64_t>>;

jasmin::Program<Instructions> MakeProgram() {
  jasmin::Program<Instructions> p;
  auto& func = p.declare("fib", 1, 1).function;
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
  return p;
}

int main() {
  // The string which is going to hold the serialized program.
  std::string s;

  {
    // Construct a program and serialize it into `s`.
    jasmin::Program<Instructions> p = MakeProgram();
    jasmin::StringWriter w(p.functions(), s);
    jasmin::Serialize(p, w);
  }

  // At this point the constructed program has been destroyed. The only remnants
  // of it are this serialization.
  std::puts("Serialized program:");
  for (size_t i = 0; i < s.size(); ++i) {
    std::printf(" %0.2x", s[i]);
    if (i % 8 == 7) { std::putchar('\n'); }
  }
  std::putchar('\n');

  {
    // Reconstitute the program previously serialized into `s` back into `p`.
    jasmin::Program<Instructions> p;
    jasmin::StringReader r(s);
    if (not jasmin::Deserialize(r, p)) { return 1; }

    uint64_t n                      = 15;
    nth::stack<jasmin::Value> stack = {n};
    p.function("fib").invoke(stack);
    std::printf("fib(%" PRIu64 ") = %" PRIu64 "\n", n,
                stack.top().as<uint64_t>());
  }

  return 0;
}