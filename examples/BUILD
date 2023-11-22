package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "fibonacci",
    srcs = ["fibonacci.cc"],
    deps = [
        "//jasmin:instruction_index",
        "//jasmin/instructions:arithmetic",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:core",
        "//jasmin:execute",
        "@asoffer_nth//nth/container:interval",
    ],
)

cc_binary(
    name = "function_state",
    srcs = ["function_state.cc"],
    deps = [
        "//jasmin/instructions:core",
        "//jasmin:execute",
    ],
)

cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    deps = [
        "//jasmin:instruction_index",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:core",
        "//jasmin:execute",
        "@asoffer_nth//nth/container:interval",
    ],
)
