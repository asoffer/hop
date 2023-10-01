package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bool",
    hdrs = ["bool.h"],
    deps = [
        "//jasmin:instruction",
    ],
)

cc_test(
    name = "bool_test",
    srcs = ["bool_test.cc"],
    deps = [
        ":bool",
        "//jasmin:testing",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_library(
    name = "core",
    hdrs = ["core.h"],
    deps = [
        "//jasmin:instruction",
    ],
)

cc_test(
    name = "core_test",
    srcs = ["core_test.cc"],
    deps = [
        ":core",
        "//jasmin:testing",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_library(
    name = "arithmetic",
    hdrs = ["arithmetic.h"],
    deps = [
        "//jasmin:instruction",
    ],
)

cc_test(
    name = "arithmetic_test",
    srcs = ["arithmetic_test.cc"],
    deps = [
        ":arithmetic",
        "//jasmin:testing",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_library(
    name = "compare",
    hdrs = ["compare.h"],
    deps = [
        "//jasmin:instruction",
    ],
)

cc_test(
    name = "compare_test",
    srcs = ["compare_test.cc"],
    deps = [
        ":compare",
        "//jasmin:testing",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_library(
    name = "stack",
    hdrs = ["stack.h"],
    deps = [
        "//jasmin:instruction",
        "//jasmin:value_stack",
        "@asoffer_nth//nth/debug",
    ],
)

cc_test(
    name = "stack_test",
    srcs = ["stack_test.cc"],
    deps = [
        ":stack",
        "//jasmin:testing",
        "@com_google_googletest//:gtest_main",
    ]
)
