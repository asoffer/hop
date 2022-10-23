package(default_visibility = ["//visibility:private"])

cc_library(
    name = "call_stack",
    hdrs = ["call_stack.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/internal:debug",
        "//jasmin/internal:function_base",
    ],
)

cc_test(
    name = "call_stack_test",
    srcs = ["call_stack_test.cc"],
    deps = [
        ":call_stack",
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
        ":instruction_pointer",
        ":value",
        ":value_stack",
    ],
)

cc_library(
    name = "function",
    hdrs = ["function.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":call_stack",
        ":instruction_pointer",
        ":instruction",
        "//jasmin/internal:debug",
        "//jasmin/internal:function_base",
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
        ":instruction_pointer",
        ":value",
        ":value_stack",
        "//jasmin/internal:attributes",
        "//jasmin/internal:function_base",
        "//jasmin/internal:type_list",
        "//jasmin/internal:type_traits",
    ],
)

cc_test(
    name = "instruction_test",
    srcs = ["instruction_test.cc"],
    deps = [
        ":instruction",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "instruction_pointer",
    hdrs = ["instruction_pointer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":op_code",
        ":value",
        "//jasmin/internal:debug",
    ],
)

cc_test(
    name = "instruction_pointer_test",
    srcs = ["instruction_pointer_test.cc"],
    deps = [
        ":instruction_pointer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "op_code",
    hdrs = ["op_code.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "//jasmin/internal:debug",
    ],
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
    name = "serialization",
    hdrs = ["serialization.h"],
    deps = [
        ":function",
        ":instruction",
        "//jasmin/internal:debug",
        "//jasmin/internal:type_list",
    ],
)

cc_test(
    name = "serialization_test",
    srcs = ["serialization_test.cc"],
    deps = [
        ":serialization",
        "//jasmin/instructions:core",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "value_stack",
    hdrs = ["value_stack.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "//jasmin/internal:debug",
        "//jasmin/internal:type_traits",
    ],
)

cc_test(
    name = "value_stack_test",
    srcs = ["value_stack_test.cc"],
    deps = [
        ":value_stack",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "value",
    hdrs = ["value.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/internal:debug",
    ],
)

cc_test(
    name = "value_test",
    srcs = ["value_test.cc"],
    deps = [
        ":value",
        "@com_google_googletest//:gtest_main",
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
