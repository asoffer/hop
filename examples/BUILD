package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "debugger",
    srcs = ["debugger.cc"],
    deps = [
        "//jasmin/core:debugger",
        "//jasmin/core:execute",
        "//jasmin/core:function",
        "//jasmin/core:program",
        "//jasmin/instructions:common",
    ],
)

cc_binary(
    name = "fibonacci",
    srcs = ["fibonacci.cc"],
    deps = [
        "//jasmin/core:execute",
        "//jasmin/core:instruction_index",
        "//jasmin/core:program",
        "//jasmin/instructions:arithmetic",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:common",
        "@asoffer_nth//nth/container:interval",
    ],
)

cc_binary(
    name = "function_state",
    srcs = ["function_state.cc"],
    deps = [
        "//jasmin/core:execute",
        "//jasmin/instructions:common",
    ],
)

cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    deps = [
        "//jasmin/core:execute",
        "//jasmin/core:instruction_index",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:common",
        "@asoffer_nth//nth/container:interval",
    ],
)
