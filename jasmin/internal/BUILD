package(default_visibility = ["//visibility:private"])

cc_library(
    name = "attributes",
    hdrs = ["attributes.h"],
    visibility = ["//jasmin:__subpackages__"],
    deps = [],
)

cc_library(
    name = "debug",
    hdrs = ["debug.h"],
    visibility = ["//jasmin:__subpackages__"],
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
    visibility = ["//jasmin:__subpackages__"],
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
    name = "type_list",
    hdrs = ["type_list.h"],
    visibility = ["//jasmin:__subpackages__"],
    deps = [
        ":type_traits",
    ],
)

cc_test(
  name = "type_list_test",
  srcs = ["type_list_test.cc"],
  deps = [
      ":type_list",
      "@com_google_googletest//:gtest_main",
  ]
)

cc_library(
    name = "type_traits",
    hdrs = ["type_traits.h"],
    visibility = ["//jasmin:__subpackages__"],
    deps = [],
)

cc_test(
  name = "type_traits_test",
  srcs = ["type_traits_test.cc"],
  deps = [
      ":type_traits",
      "@com_google_googletest//:gtest_main",
  ]
)
