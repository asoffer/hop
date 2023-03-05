package(default_visibility = ["//jasmin:__subpackages__"])

cc_library(
    name = "attributes",
    hdrs = ["attributes.h"],
    deps = [],
)

cc_library(
    name = "debug",
    hdrs = ["debug.h"],
    deps = [],
)

cc_test(
    name = "debug_test",
    srcs = ["debug_test.cc"],
    deps = [
        ":debug",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "function_base",
    hdrs = ["function_base.h"],
    deps = [
        "//jasmin:instruction_pointer",
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
    name = "type_traits",
    hdrs = ["type_traits.h"],
    deps = [
        "@asoffer_nth//nth/meta:concepts",
    ],
)

cc_test(
  name = "type_traits_test",
  srcs = ["type_traits_test.cc"],
  deps = [
      ":type_traits",
      "@com_google_googletest//:gtest_main",
  ]
)
