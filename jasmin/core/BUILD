package(default_visibility = ["//visibility:private"])

cc_library(
    name = "debugger",
    hdrs = ["debugger.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":function",
        ":instruction",
        ":program",
        ":value",
        "@asoffer_nth//nth/base:attributes",
        "@asoffer_nth//nth/debug",
        "@com_google_absl//absl/container:node_hash_map",
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
        "//jasmin/core/internal:function_base",
        "//jasmin/core/internal:instruction_traits",
        "@asoffer_nth//nth/container:interval",
    ],
)

cc_test(
    name = "function_test",
    srcs = ["function_test.cc"],
    deps = [
        ":function",
        "@asoffer_nth//nth/test:main",
    ],
)

cc_library(
    name = "instruction",
    hdrs = ["instruction.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "//jasmin/core/internal:function_base",
        "//jasmin/core/internal:function_state",
        "//jasmin/core/internal:instruction_traits",
        "@asoffer_nth//nth/base:attributes",
        "@asoffer_nth//nth/base:pack",
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
    name = "metadata",
    hdrs = ["metadata.h"],
    srcs = ["metadata.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":instruction",
        ":value",
        "//jasmin/core/internal:instruction_traits",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/utility:no_destructor",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "metadata_test",
    srcs = ["metadata_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":metadata",
        "@asoffer_nth//nth/test:main",
    ],
)

cc_library(
    name = "program",
    hdrs = ["program.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":function",
        ":instruction",
        ":value",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/utility:iterator_range",
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
        "@asoffer_nth//nth/test:main",
    ],
)
