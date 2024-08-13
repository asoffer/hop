package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "debugger",
    srcs = ["debugger.cc"],
    deps = [
        "//jasmin/core:debugger",
        "//jasmin/core:function",
        "//jasmin/core:program_fragment",
        "//jasmin/instructions:common",
        "@nth_cc//nth/container:stack",
    ],
)

cc_binary(
    name = "fibonacci",
    srcs = ["fibonacci.cc"],
    deps = [
        "//jasmin/core:instruction_index",
        "//jasmin/core:program_fragment",
        "//jasmin/instructions:arithmetic",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:common",
        "@nth_cc//nth/container:interval",
        "@nth_cc//nth/container:stack",
    ],
)

cc_binary(
    name = "function_state",
    srcs = ["function_state.cc"],
    deps = [
        "//jasmin/instructions:common",
        "@nth_cc//nth/container:stack",
    ],
)

cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    deps = [
        "//jasmin/core:instruction_index",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:common",
        "@nth_cc//nth/container:interval",
        "@nth_cc//nth/container:stack",
    ],
)
