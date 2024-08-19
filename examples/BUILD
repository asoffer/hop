package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "debugger",
    srcs = ["debugger.cc"],
    deps = [
        "//hop/core:debugger",
        "//hop/core:function",
        "//hop/core:program_fragment",
        "//hop/instructions:common",
        "@nth_cc//nth/container:stack",
    ],
)

cc_binary(
    name = "fibonacci",
    srcs = ["fibonacci.cc"],
    deps = [
        "//hop/core:instruction_index",
        "//hop/core:program_fragment",
        "//hop/instructions:arithmetic",
        "//hop/instructions:compare",
        "//hop/instructions:common",
        "@nth_cc//nth/container:interval",
        "@nth_cc//nth/container:stack",
    ],
)

cc_binary(
    name = "function_state",
    srcs = ["function_state.cc"],
    deps = [
        "//hop/instructions:common",
        "@nth_cc//nth/container:stack",
    ],
)

cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    deps = [
        "//hop/core:instruction_index",
        "//hop/instructions:compare",
        "//hop/instructions:common",
        "@nth_cc//nth/container:interval",
        "@nth_cc//nth/container:stack",
    ],
)
