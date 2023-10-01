package(default_visibility = ["//visibility:private"])

cc_library(
    name = "call_stack",
    hdrs = ["call_stack.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/internal:function_base",
        "@asoffer_nth//nth/debug",
    ],
)

cc_test(
    name = "call_stack_test",
    srcs = ["call_stack_test.cc"],
    deps = [
        ":call_stack",
        "@com_google_googletest//:gtest_main",
        "@asoffer_nth//nth/debug/log",
        "@asoffer_nth//nth/debug/log:stderr_log_sink",
    ],
)

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
        ":call_stack",
        ":function",
        ":instruction",
        ":value",
        ":value_stack",
        "@asoffer_nth//nth/debug",
    ],
)

cc_library(
    name = "execution_state",
    hdrs = ["execution_state.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/internal:type_traits",
        "@asoffer_nth//nth/meta:sequence",
        "@asoffer_nth//nth/meta:type",
    ],
)

cc_library(
    name = "function",
    hdrs = ["function.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":call_stack",
        ":instruction",
        ":value",
        "//jasmin/internal:function_base",
        "//jasmin/internal:instruction_traits",
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
        ":call_stack",
        ":execution_state",
        ":value",
        ":value_stack",
        "//jasmin/internal:function_base",
        "//jasmin/internal:function_state",
        "//jasmin/internal:type_traits",
        "@asoffer_nth//nth/base:attributes",
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
        "@asoffer_nth//nth/debug/log",
        "@asoffer_nth//nth/debug/log:stderr_log_sink",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "op_code",
    hdrs = ["op_code.h"],
    visibility = ["//visibility:public"],
    deps = [],
)

cc_test(
    name = "op_code_test",
    srcs = ["op_code_test.cc"],
    deps = [
        ":op_code",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "value_stack",
    hdrs = ["value_stack.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "//jasmin/internal:type_traits",
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
