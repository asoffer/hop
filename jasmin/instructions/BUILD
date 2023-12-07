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
        "@asoffer_nth//nth/container:stack",
        "@asoffer_nth//nth/test:main",
    ]
)

cc_library(
    name = "common",
    hdrs = ["common.h"],
    deps = [
        "//jasmin/core:instruction",
    ],
)

cc_test(
    name = "common_test",
    srcs = ["common_test.cc"],
    deps = [
        ":common",
        "//jasmin:testing",
        "@asoffer_nth//nth/container:stack",
        "@asoffer_nth//nth/test:main",
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
        "@asoffer_nth//nth/container:stack",
        "@asoffer_nth//nth/test:main",
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
        "@asoffer_nth//nth/container:stack",
        "@asoffer_nth//nth/test:main",
    ]
)

cc_library(
    name = "stack",
    hdrs = ["stack.h"],
    deps = [
        "//jasmin/core:instruction",
        "//jasmin/core:value",
        "@asoffer_nth//nth/debug",
    ],
)

cc_test(
    name = "stack_test",
    srcs = ["stack_test.cc"],
    deps = [
        ":stack",
        "//jasmin:testing",
        "@asoffer_nth//nth/test:main",
    ]
)
