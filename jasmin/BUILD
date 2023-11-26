package(default_visibility = ["//visibility:private"])

cc_library(
    name = "debug",
    hdrs = ["debug.h"],
    srcs = ["debug.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":instruction",
        ":function",
        ":value",
        ":value_stack",
        "//jasmin/internal:instruction_traits",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/meta:type",
    ],
)

cc_test(
    name = "debug_test",
    srcs = ["debug_test.cc"],
    deps = [
        ":debug",
        ":testing",
        "//jasmin/instructions:core",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "execute",
    hdrs = ["execute.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":function",
        ":instruction",
        ":value",
        ":value_stack",
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
        ":value_stack",
        "//jasmin/internal:call_stack",
        "//jasmin/internal:function_base",
        "//jasmin/internal:function_state",
        "//jasmin/internal:instruction_traits",
        "@asoffer_nth//nth/base:attributes",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/meta:sequence",
        "@asoffer_nth//nth/meta:type",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "instruction_test",
    srcs = ["instruction_test.cc"],
    deps = [
        ":instruction",
        "@asoffer_nth//nth/debug/log",
        "@asoffer_nth//nth/debug/log:stderr_log_sink",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "instruction_index",
    hdrs = ["instruction_index.h"],
    visibility = ["//visibility:public"],
    deps = [],
)

cc_library(
    name = "value_stack",
    hdrs = ["value_stack.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "@asoffer_nth//nth/debug",
    ],
)

cc_test(
    name = "value_stack_test",
    srcs = ["value_stack_test.cc"],
    deps = [
        ":value_stack",
        "@asoffer_nth//nth/debug/log",
        "@asoffer_nth//nth/debug/log:stderr_log_sink",
        "@com_google_googletest//:gtest_main",
    ],
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
        ":value_stack",
    ],
)
