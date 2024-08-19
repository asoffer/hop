package(default_visibility = ["//visibility:private"])

cc_library(
    name = "function_emitter",
    hdrs = ["function_emitter.h"],
    srcs = ["function_emitter.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":location_map",
        "//hop/compile:compiled_function",
        "//hop/core:instruction",
        "//hop/ssa",
        "@nth_cc//nth/meta:type",
        "@nth_cc//nth/debug",
    ],
)

cc_library(
    name = "location_map",
    hdrs = ["location_map.h"],
    visibility = ["//visibility:public"],
    deps = [
    ],
)
