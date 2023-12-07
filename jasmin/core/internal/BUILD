package(default_visibility = ["//jasmin/core:__subpackages__"])

cc_library(
    name = "function_base",
    hdrs = ["function_base.h"],
    deps = [
        "//jasmin/core:instruction_index",
        "//jasmin/core:value",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/container:interval",
    ],
)

cc_test(
    name = "function_base_test",
    srcs = ["function_base_test.cc"],
    deps = [
        ":function_base",
        "@asoffer_nth//nth/test:main",
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
    deps = [
        ":function_state",
        "//jasmin/core:value",
        "@asoffer_nth//nth/meta:concepts",
        "@asoffer_nth//nth/meta:type",
    ],
)

cc_test(
    name = "instruction_traits_test",
    srcs = ["instruction_traits_test.cc"],
    deps = [
        ":instruction_traits",
        "//jasmin/core:instruction",
        "@asoffer_nth//nth/test:main",
    ],
)
