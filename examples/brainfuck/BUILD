package(default_visibility = ["//visibility:public"])

cc_library(
    name = "build",
    hdrs = ["build.h"],
    srcs = ["build.cc"],
    deps = [
        ":instructions",
        "//hop/core:function",
    ],
)

cc_library(
    name = "file",
    hdrs = ["file.h"],
    srcs = ["file.cc"],
    deps = [
        "@nth_cc//nth/debug",
        "@nth_cc//nth/io:file",
        "@nth_cc//nth/io:file_path",
    ],
)

cc_library(
    name = "instructions",
    hdrs = ["instructions.h"],
    deps = [
        "//hop/core:instruction",
    ],
)

cc_binary(
    name = "interpreter",
    srcs = ["interpreter.cc"],
    deps = [
        ":build",
        ":file",
        ":instructions",
        "//hop/core:function",
        "@nth_cc//nth/debug/log:stderr_log_sink",
    ],
)

cc_binary(
    name = "jit",
    srcs = ["jit.cc"],
    deps = [
        ":build",
        ":file",
        ":instructions",
        ":x64_code_generator",
        "//hop/core:function",
        "//hop/ssa",
        "//hop/compile/x64:function_emitter",
        "@nth_cc//nth/dynamic:jit_function",
    ],
)

cc_library(
    name = "x64_code_generator",
    hdrs = ["x64_code_generator.h"],
    deps = [
        "//hop/core:instruction",
        "//hop/compile/x64:function_emitter",
        "//hop/compile/x64:location_map",
    ],
)
