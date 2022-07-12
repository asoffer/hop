package(default_visibility = ["//visibility:public"])

cc_library(
    name = "instruction",
    hdrs = ["instruction.h"],
    deps = [
        "//jasmin/internal:attributes",
        "//jasmin/internal:type_traits",
        "//jasmin:value",
        "//jasmin:value_stack",
    ],
)

cc_library(
    name = "core",
    hdrs = ["core.h"],
    deps = [
        ":instruction",
    ],
)

cc_library(
    name = "arithmetic",
    hdrs = ["arithmetic.h"],
    deps = [
        ":instruction",
    ],
)

cc_library(
    name = "compare",
    hdrs = ["compare.h"],
    deps = [
        ":instruction",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":instruction",
        ":arithmetic",
        ":compare",
        ":core",
    ],
)
