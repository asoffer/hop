package(default_visibility = ["//visibility:public"])

cc_library(
    name = "build",
    hdrs = ["build.h"],
    srcs = ["build.cc"],
    deps = [
        ":instructions",
        "//jasmin/core:function",
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
        "//jasmin/core:instruction",
    ],
)

cc_binary(
    name = "interpreter",
    srcs = ["interpreter.cc"],
    deps = [
        ":build",
        ":file",
        ":instructions",
        "//jasmin/core:function",
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
        "//jasmin/core:function",
        "//jasmin/ssa",
        "//jasmin/compile/x64:function_emitter",
        "@nth_cc//nth/dynamic:jit_function",
    ],
)

cc_library(
    name = "x64_code_generator",
    hdrs = ["x64_code_generator.h"],
    deps = [
        "//jasmin/core:instruction",
        "//jasmin/compile/x64:function_emitter",
        "//jasmin/compile/x64:location_map",
    ],
)
