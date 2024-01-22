package(default_visibility = ["//visibility:private"])

cc_library(
    name = "testing",
    testonly = True,
    hdrs = ["testing.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/core:function",
        "//jasmin/core:instruction",
        "//jasmin/core:value",
        "@nth_cc//nth/container:stack",
    ],
)
