package(default_visibility = ["//visibility:private"])

cc_library(
    name = "execute",
    hdrs = ["execute.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":function",
        ":instruction",
        ":value",
        "@asoffer_nth//nth/container:stack",
        "@asoffer_nth//nth/debug",
    ],
)

cc_library(
    name = "function",
    hdrs = ["function.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":instruction",
        ":instruction_index",
        ":value",
        "//jasmin/internal:function_base",
        "//jasmin/internal:instruction_traits",
        "@asoffer_nth//nth/container:interval",
    ],
)

cc_test(
    name = "function_test",
    srcs = ["function_test.cc"],
    deps = [
        ":execute",
        ":function",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "instruction",
    hdrs = ["instruction.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "//jasmin/internal:function_base",
        "//jasmin/internal:function_state",
        "//jasmin/internal:instruction_traits",
        "@asoffer_nth//nth/base:attributes",
        "@asoffer_nth//nth/container:stack",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/meta:sequence",
        "@asoffer_nth//nth/meta:type",
    ],
)

cc_test(
    name = "instruction_test",
    srcs = ["instruction_test.cc"],
    deps = [
        ":instruction",
        "@asoffer_nth//nth/test:main",
    ],
)

cc_library(
    name = "instruction_index",
    hdrs = ["instruction_index.h"],
    visibility = ["//visibility:public"],
    deps = [],
)

cc_library(
    name = "value",
    hdrs = ["value.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/configuration:impl",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/meta:type",
    ],
)

cc_test(
    name = "value_test",
    srcs = ["value_test.cc"],
    deps = [
        ":value",
        "@com_google_googletest//:gtest_main",
        "@asoffer_nth//nth/debug/log",
        "@asoffer_nth//nth/debug/log:stderr_log_sink",
    ],
)

cc_library(
    name = "testing",
    testonly = True,
    hdrs = ["testing.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":instruction",
        ":execute",
        ":function",
        ":value",
        "@asoffer_nth//nth/container:stack",
    ],
)
