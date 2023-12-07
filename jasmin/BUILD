package(default_visibility = ["//visibility:private"])

cc_library(
    name = "testing",
    testonly = True,
    hdrs = ["testing.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/core:execute",
        "//jasmin/core:function",
        "//jasmin/core:instruction",
        "//jasmin/core:value",
        "@asoffer_nth//nth/container:stack",
    ],
)
