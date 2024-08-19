#include <cstdio>

#include "examples/brainfuck/build.h"
#include "examples/brainfuck/file.h"
#include "examples/brainfuck/instructions.h"
#include "examples/brainfuck/x64_code_generator.h"
#include "hop/compile/compiled_function.h"
#include "hop/ssa/ssa.h"
#include "nth/dynamic/jit_function.h"

int main(int argc, char* argv[]) {
  // Load Brainfuck program source text.
  if (argc != 2) { return 1; }
  std::string contents = bf::LoadFileContentsOrDie(argv[1]);

  // Constructs a function from the program source text.
  std::variant fn_or_parse_error = bf::BuildHopFunction(contents);

  if (auto* error = std::get_if<bf::parse_error>(&fn_or_parse_error)) {
    std::fprintf(stderr, "Parse error on line %d (column %d).\n", error->line,
                 error->column);
    return 1;
  }

  auto& f = std::get<hop::Function<bf::Instructions>>(fn_or_parse_error);

  hop::CompiledFunction code;
  {  // Generate executable code for the function, storing it in `code`.
    bf::X64CodeGenerator gen;
    hop::x64::FunctionEmitter emitter(nth::type<bf::Instructions>, gen);
    emitter.emit(hop::SsaFunction(f), code);
  }

  // Construct a JIT-compiled function from the code.
  nth::jit_function<void()> jitted_fn(code);

  // Invoke the function.
  jitted_fn();

  return 0;
}
