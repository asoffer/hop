package(default_visibility = ["//visibility:private"])

cc_library(
    name = "debug",
    hdrs = ["debug.h"],
    deps = [],
)

cc_library(
    name = "value_stack",
    hdrs = ["value_stack.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
    ],
)

cc_library(
    name = "value",
    hdrs = ["value.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":debug",
    ],
)
