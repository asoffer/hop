#include <cstdio>

#include "examples/brainfuck/build.h"
#include "examples/brainfuck/file.h"
#include "examples/brainfuck/instructions.h"
#include "jasmin/compile/x64/code_generator.h"
#include "jasmin/jit/function.h"
#include "jasmin/ssa/ssa.h"

int main(int argc, char* argv[]) {
  // Load Brainfuck program source text.
  if (argc != 2) { return 1; }
  std::string contents = bf::LoadFileContentsOrDie(argv[1]);

  // Constructs a function from the program source text.
  std::variant fn_or_parse_error = bf::BuildJasminFunction(contents);

  if (auto* error = std::get_if<bf::parse_error>(&fn_or_parse_error)) {
    std::fprintf(stderr, "Parse error on line %d (column %d).\n", error->line,
                 error->column);
    return 1;
  }

  auto& f = std::get<jasmin::Function<bf::Instructions>>(fn_or_parse_error);

  // Generate executable code for the function, storing it in `code`.
  jasmin::CompiledFunction code;
  jasmin::x64::CodeGenerator gen(nth::type<bf::Instructions>);
  gen.function(jasmin::SsaFunction(f), code);

  // Construct a JIT-compiled function from the code.
  jasmin::JitFunction<void()> jitted_fn(code);

  // Invoke the function.
  jitted_fn();

  return 0;
}
