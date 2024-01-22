package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bool",
    hdrs = ["bool.h"],
    deps = [
        "//jasmin/core:instruction",
    ],
)

cc_test(
    name = "bool_test",
    srcs = ["bool_test.cc"],
    deps = [
        ":bool",
        "//jasmin:testing",
        "@nth_cc//nth/container:stack",
        "@nth_cc//nth/test:main",
    ]
)

cc_library(
    name = "common",
    hdrs = ["common.h"],
    deps = [
        "//jasmin/core:instruction",
        "//jasmin/ssa",
        "//jasmin/ssa:register_coalescer",
    ],
)

cc_test(
    name = "common_test",
    srcs = ["common_test.cc"],
    deps = [
        ":common",
        "//jasmin:testing",
        "@nth_cc//nth/container:stack",
        "@nth_cc//nth/test:main",
    ]
)

cc_library(
    name = "arithmetic",
    hdrs = ["arithmetic.h"],
    deps = [
        "//jasmin/core:instruction",
    ],
)

cc_test(
    name = "arithmetic_test",
    srcs = ["arithmetic_test.cc"],
    deps = [
        ":arithmetic",
        "//jasmin:testing",
        "@nth_cc//nth/container:stack",
        "@nth_cc//nth/test:main",
    ]
)

cc_library(
    name = "compare",
    hdrs = ["compare.h"],
    deps = [
        "//jasmin/core:instruction",
    ],
)

cc_test(
    name = "compare_test",
    srcs = ["compare_test.cc"],
    deps = [
        ":compare",
        "//jasmin:testing",
        "@nth_cc//nth/container:stack",
        "@nth_cc//nth/test:main",
    ]
)

cc_library(
    name = "stack",
    hdrs = ["stack.h"],
    deps = [
        "//jasmin/core:instruction",
        "//jasmin/core:value",
        "@nth_cc//nth/debug",
    ],
)

cc_test(
    name = "stack_test",
    srcs = ["stack_test.cc"],
    deps = [
        ":stack",
        "//jasmin:testing",
        "@nth_cc//nth/test:main",
    ]
)
