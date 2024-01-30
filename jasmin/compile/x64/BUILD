package(default_visibility = ["//visibility:private"])

cc_library(
    name = "code_generator",
    hdrs = ["code_generator.h"],
    srcs = ["code_generator.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":location_map",
        "//jasmin/compile:compiled_function",
        "//jasmin/core:instruction",
        "//jasmin/ssa",
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
