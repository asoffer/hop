# Brainfuck Example

## Introduction

[Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) is an esoteric programming
in which only 8 characters have any meaning in source code: `<`, `>`, `+`, `[`,
`]`, `,`, and `.`.

This example directory shows an interpreter and a [JIT (Just-In-Time) compiler](
https://en.wikipedia.org/wiki/Just-in-time_compilation) for Brainfuck. While
most well-known programming languages have complexity both in parsing the source
text of the program and in generating executable code, Brainfuck has almost no
complexity parsing complexity: Any sequence of the above characters with
matching `[` and `]` is a valid Brainfuck program. For this reason, Brainfuck is
a particularly good example to show writing the backend of interpreters and
compilers.

## Running the code

You can build the interpter or JIT-compiler with these commands:

```
$ bazel build -c opt //examples/brainfuck:interpreter
```

```
$ bazel build -c opt //examples/brainfuck:jit
```

One can then execute any Brainfuck program with:

```
$ ./bazel-bin/examples/brainfuck/interpreter path/to/program.bf  # Interpret the program.
$ ./bazel-bin/examples/brainfuck/interpreter path/to/program.bf  # JIT compile and run the program.
```

## Understanding the Interpreter

The interpreter, defined in `examples/brainfuck/interpreter.cc` parses the
program, producing a `jasmin::Function<>` that can be directly executed via
Jasmin's interpreter. There's not much else going on here. Every object of
type `jasmin::Function<>` has a member function named `invoke` that can be
called to interpret the function.

## Understanding the JIT compiler

The JIT compiler, defined in `examples/brainfuck/jit.cc` parses the
program, producing a `jasmin::Function<>`, just as with the interpreter.
However, rather than interpreting the function in that form, it is passed to
a code-generator to produce a `jasmin::CompiledFunction`. A compiled function
cannot by itself be executed, but can be passed as an argument to the
`jasmin::JitFunction<>` constructor. The resulting `jasmin::JitFunction<>` can
be invoked as any other normal C++ function.

It may seem strange that we have the intermediate `jasmin::CompiledFunction`
type. The reason for the separation is twofold. First, for security reasons we
do not want the contents of a `jasmin::JitFunction<>` to ever be simultaneously
writable and executable. To ensure this, `jasmin::JitFunction<>`s are not
modifiable after construction. Second, the `jasmin::CompiledFunction` can be
used for compilation other than JITs. While a `jasmin::JitFunction<>` must be
constructed with compiled code suitable for the executing architecture, one
could cross-compile functions via `jasmin::CompiledFunction` without JIT-ing
them.
