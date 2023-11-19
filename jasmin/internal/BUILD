package(default_visibility = ["//jasmin:__subpackages__"])

cc_library(
    name = "function_base",
    hdrs = ["function_base.h"],
    deps = [
        "//jasmin:instruction_index",
        "//jasmin:value",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/container:interval",
    ],
)

cc_test(
    name = "function_base_test",
    srcs = ["function_base_test.cc"],
    deps = [
      ":function_base",
      "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "function_state",
    hdrs = ["function_state.h"],
    deps = [],
)

cc_library(
    name = "instruction_traits",
    hdrs = ["instruction_traits.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin:instruction",
        "@asoffer_nth//nth/meta:type",
    ],
)

cc_test(
    name = "instruction_traits_test",
    srcs = ["instruction_traits_test.cc"],
    deps = [
        ":instruction_traits",
        "@com_google_googletest//:gtest_main",
    ],
)
