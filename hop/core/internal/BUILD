package(default_visibility = ["//hop/core:__subpackages__"])

cc_library(
    name = "frame",
    hdrs = ["frame.h"],
    deps = [
        "//hop/core:value",
    ],
)

cc_library(
    name = "function_base",
    hdrs = ["function_base.h"],
    deps = [
        ":frame",
        ":function_state",
        "//hop/core:instruction_index",
        "//hop/core:value",
        "@nth_cc//nth/debug",
        "@nth_cc//nth/container:interval",
        "@nth_cc//nth/container:stack",
    ],
)

cc_library(
    name = "function_forward",
    hdrs = ["function_forward.h"],
    deps = [],
)

cc_test(
    name = "function_base_test",
    srcs = ["function_base_test.cc"],
    deps = [
        ":function_base",
        "@nth_cc//nth/test:main",
    ],
)

cc_library(
    name = "function_state",
    hdrs = ["function_state.h"],
    deps = [],
)

cc_library(
    name = "instruction_traits",
    hdrs = ["instruction_traits.h"],
    deps = [
        ":function_state",
        "//hop/core:input",
        "//hop/core:output",
        "//hop/core:value",
        "@nth_cc//nth/format",
        "@nth_cc//nth/meta:concepts",
        "@nth_cc//nth/meta:type",
        "@nth_cc//nth/io/deserialize",
        "@nth_cc//nth/io/serialize",
    ],
)

cc_test(
    name = "instruction_traits_test",
    srcs = ["instruction_traits_test.cc"],
    deps = [
        ":instruction_traits",
        "//hop/core:instruction",
        "@nth_cc//nth/test:main",
    ],
)
