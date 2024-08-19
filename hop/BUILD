package(default_visibility = ["//visibility:private"])

cc_library(
    name = "testing",
    testonly = True,
    hdrs = ["testing.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//hop/core:function",
        "//hop/core:instruction",
        "//hop/core:value",
        "@nth_cc//nth/container:stack",
    ],
)
