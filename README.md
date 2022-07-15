# Jasmin

## Introduction

Jasmin is a stack-machine-based byte-code interpreter. Jasmin is intended to be
both fast and customizable, aiming to be a target for higher-level interpreted
programming languages.

[Stack machine](https://en.wikipedia.org/wiki/Stack_machine) interpreters differ
from more traditional register-based interpreters in that they do not use
registers for passing arguments to instructions. Instructions execute by pushing
and popping values from the top of a stack.

Perhaps the most distinguishing feature of Jasmin over other stack-machine
interpreters is that Jasmin does not have a fixed instruction set. Rather, users
are expected to define their own instructions that are best suited to their use
case. Jasmin provides [common instructions](jasmin/instructions/) which one can,
but is not required to, use in their instruction set.

## Tutorials and Examples

* Examples can be found in the [examples directory](jasmin/examples/), though
  there are not many examples yet.
* We do not yet have a tutorial, though it is on our radar. For now, reading
  examples and the source code are your best bet for learning how to use these
  libraries.

## Building from Source

Jasmin is a collection of C++20 libraries, and relies heavily on Bazel for
building and testing. You will need a C++20-compliant compiler and a relatively
recent copy of [Bazel](https://bazel.build/). Specifically,

* Bazel version 5.2.0 or greater
* Clang 13.0.0

Jasmin libraries is intended to be used from your C++ code and always built from
source. For this reason, Jasmin does not have any installation or setup process.
If you would like to download and play around with the examples, they can be
built via these commands:

```
$ git clone https://github.com/asoffer/jasmin.git
$ cd jasmin
$ bazel build -c opt //examples:fibonacci
```

If you would like to include Jasmin in your own Bazel based project, you can
use an [`http_archive`](https://bazel.build/rules/lib/repo/http) rule in your
`WORKSPACE` file. See Bazel's tutorials for how to do this. A good starting
point will be to copy this into your `WORKSPACE` file:

```
http_archive(
  name = "asoffer_jasmin",
  urls = ["https://github.com/asoffer/jasmin/archive/<<hash>>.zip"],
  strip_prefix = "jasmin-<<hash>>",
  sha256 = "<<sha>>",
)
```
where `<<hash>>` and `<sha>>` are replaced with the appropriate Git hash and
checksum as desired.

Any compilation action my also optionally be passed the argument
`--copt=-DJASMIN_DEBUG` (regardless of whether the build is `opt`, `fastbuild`,
or `dbg`. At a performance cost, this will enable debugging facilities that catch
common mistakes relying on behavior that is not defined by Jasmin's APIs.

## Continuous Integration

Coming soon...

Currently Jasmin is only tested on ARM-based macOS platforms. We are open (in
fact, ecstatic) to receive pull requests that support more environments and
platforms (new hardware, operating systems, compilers, etc).  However, we are
unlikely to prioritize support for anything other than the few most-recent
versions of any particular operating system or compiler.

## Long Term Support

As it is currently in the early exploratory stages of development, we are not
ready to make any guarantees, even for short-term stability.

In the future we intend to provide significant long-term support, but in a
somewhat unconventional way: Jasmin will never make guarantees of either API
stability or ABI stability. While we aim to make API changes unobtrusive, we
will add, change, or remove APIs. This enables us to make to always be able
to provide the best possible software for our users.

Though any change can be a breaking change, we aim to advertize the potential
breakge (e.g., removing a public function). Moreover, we aim to make such
changes in such a way that upgrading the version of Jasmin can be done in a
change of its own, separate from any refactoring that may be required to make
the upgrade. Concretely, this means that if we want to, for example, rename a
function, we will add a function with the new name and mark the old one as
deprecated in one change, and remove the deprecated function in a separate
change.
