package(default_visibility = ["//visibility:public"])

cc_library(
    name = "core",
    hdrs = ["core.h"],
    deps = [
        "//jasmin:execute",
    ],
)

cc_library(
    name = "arithmetic",
    hdrs = ["arithmetic.h"],
    deps = [
        "//jasmin:execute",
    ],
)

cc_library(
    name = "compare",
    hdrs = ["compare.h"],
    deps = [
        "//jasmin:execute",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":arithmetic",
        ":compare",
        ":core",
        "//jasmin:execute",
    ],
)
