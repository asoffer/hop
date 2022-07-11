package(default_visibility = ["//visibility:public"])

cc_library(
    name = "instruction",
    hdrs = ["instruction.h"],
    deps = [
        ":value",
        ":value_stack",
    ],
)

cc_library(
    name = "core",
    hdrs = ["core.h"],
    deps = [
      ":instruction",
    ],
)

cc_library(
    name = "arithmetic",
    hdrs = ["arithmetic.h"],
    deps = [
      ":instruction",
    ],
)

cc_library(
    name = "compare",
    hdrs = ["compare.h"],
    deps = [
      ":instruction",
    ],
)

cc_library(
    name = "value_stack",
    hdrs = ["value_stack.h"],
    deps = [
      ":value",
    ],
)

cc_library(
    name = "value",
    hdrs = ["value.h"],
    deps = [],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
      ":instruction",
      ":arithmetic",
      ":compare",
      ":core",
    ],
)
