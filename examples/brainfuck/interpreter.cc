#include <cstdio>

#include "examples/brainfuck/build.h"
#include "examples/brainfuck/file.h"
#include "examples/brainfuck/instructions.h"
#include "nth/debug/log/stderr_log_sink.h"

int main(int argc, char* argv[]) {
  nth::RegisterLogSink(nth::stderr_log_sink);

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

  // Create an empty stack, because `f` takes to arguments.
  nth::stack<jasmin::Value> stack;

  // Invoke the function.
  f.invoke(stack);

  return 0;
}
