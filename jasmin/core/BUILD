package(default_visibility = ["//visibility:private"])

cc_library(
    name = "debugger",
    hdrs = ["debugger.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":function",
        ":instruction",
        ":program_fragment",
        ":value",
        "@com_google_absl//absl/container:node_hash_map",
        "@nth_cc//nth/base:attributes",
        "@nth_cc//nth/debug",
    ],
)

cc_library(
    name = "function",
    hdrs = ["function.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":instruction",
        ":instruction_index",
        ":metadata",
        ":value",
        "//jasmin/core/internal:function_base",
        "//jasmin/core/internal:instruction_traits",
        "@nth_cc//nth/container:interval",
        "@nth_cc//nth/io/serialize",
        "@nth_cc//nth/io/serialize:deserialize",
    ],
)

cc_test(
    name = "function_test",
    srcs = ["function_test.cc"],
    deps = [
        ":function",
        "@nth_cc//nth/test:main",
    ],
)

cc_library(
    name = "input",
    hdrs = ["input.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "@nth_cc//nth/meta:sequence",
        "@nth_cc//nth/meta:type",
    ],
)

cc_test(
    name = "input_test",
    srcs = ["input_test.cc"],
    deps = [
        ":input",
        "@nth_cc//nth/test:main",
    ],
)

cc_library(
    name = "instruction",
    hdrs = ["instruction.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":input",
        ":output",
        ":value",
        "//jasmin/core/internal:function_base",
        "//jasmin/core/internal:function_state",
        "//jasmin/core/internal:instruction_traits",
        "@nth_cc//nth/base:attributes",
        "@nth_cc//nth/base:pack",
        "@nth_cc//nth/container:stack",
        "@nth_cc//nth/debug",
        "@nth_cc//nth/meta:sequence",
        "@nth_cc//nth/meta:type",
    ],
)

cc_test(
    name = "instruction_test",
    srcs = ["instruction_test.cc"],
    deps = [
        ":instruction",
        "@nth_cc//nth/test:main",
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
        "@com_google_absl//absl/container:flat_hash_map",
        "@nth_cc//nth/debug",
        "@nth_cc//nth/utility:no_destructor",
    ],
)

cc_test(
    name = "metadata_test",
    srcs = ["metadata_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":metadata",
        "@nth_cc//nth/test:main",
    ],
)

cc_library(
    name = "output",
    hdrs = ["output.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "@nth_cc//nth/meta:sequence",
        "@nth_cc//nth/meta:type",
    ],
)

cc_test(
    name = "output_test",
    srcs = ["output_test.cc"],
    deps = [
        ":output",
        "@nth_cc//nth/test:main",
    ],
)

cc_library(
    name = "program_fragment",
    hdrs = ["program_fragment.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":function",
        ":instruction",
        ":value",
        "@nth_cc//nth/debug",
        "@nth_cc//nth/io/serialize",
        "@nth_cc//nth/io/serialize:deserialize",
        "@nth_cc//nth/utility:iterator_range",
    ],
)

cc_library(
    name = "serialization",
    hdrs = ["serialization.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":function",
        ":instruction",
        ":metadata",
        ":program_fragment",
        ":value",
        "@nth_cc//nth/io/serialize",
        "@nth_cc//nth/io/serialize:deserialize",
    ],
)

cc_test(
    name = "serialization_test",
    srcs = ["serialization_test.cc"],
    deps = [
        ":serialization",
        "//jasmin/instructions:common",
        "@nth_cc//nth/io/reader:string",
        "@nth_cc//nth/io/writer:string",
        "@nth_cc//nth/test:main",
    ],
)

cc_library(
    name = "value",
    hdrs = ["value.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/configuration:impl",
        "@nth_cc//nth/debug",
        "@nth_cc//nth/meta:type",
    ],
)

cc_test(
    name = "value_test",
    srcs = ["value_test.cc"],
    deps = [
        ":value",
        "@nth_cc//nth/test:main",
    ],
)
