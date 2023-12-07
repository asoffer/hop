package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "fibonacci",
    srcs = ["fibonacci.cc"],
    deps = [
        "//jasmin/core:execute",
        "//jasmin/core:instruction_index",
        "//jasmin/instructions:arithmetic",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:core",
        "@asoffer_nth//nth/container:interval",
    ],
)

cc_binary(
    name = "function_state",
    srcs = ["function_state.cc"],
    deps = [
        "//jasmin/core:execute",
        "//jasmin/instructions:core",
    ],
)

cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    deps = [
        "//jasmin/core:execute",
        "//jasmin/core:instruction_index",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:core",
        "@asoffer_nth//nth/container:interval",
    ],
)
