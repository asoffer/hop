package(default_visibility = ["//visibility:private"])

cc_library(
    name = "deserialize",
    hdrs = ["deserialize.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":reader",
        "//jasmin/core:instruction",
        "//jasmin/core:metadata",
        "@asoffer_nth//nth/meta:type",
        "@asoffer_nth//nth/meta:sequence",
    ],
)

cc_library(
    name = "reader",
    hdrs = ["reader.h"],
)

cc_test(
    name = "round_trip_test",
    srcs = ["round_trip_test.cc"],
    deps = [
        ":deserialize",
        ":serialize",
        ":string_reader",
        ":string_writer",
        ":reader",
        ":writer",
        "//jasmin/instructions:common",
        "@asoffer_nth//nth/test:main",
    ],
)

cc_library(
    name = "serialize",
    hdrs = ["serialize.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":writer",
        "//jasmin/core:function",
        "//jasmin/core:instruction",
        "//jasmin/core:metadata",
        "//jasmin/core:program",
        "@asoffer_nth//nth/meta:type",
        "@asoffer_nth//nth/meta:sequence",
    ],
)

cc_library(
    name = "string_reader",
    hdrs = ["string_reader.h"],
    srcs = ["string_reader.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":reader",
        "//jasmin/core:function", 
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "string_reader_test",
    srcs = ["string_reader_test.cc"],
    deps = [
        ":string_reader",
        "@asoffer_nth//nth/test:main",
    ],
)

cc_library(
    name = "string_writer",
    hdrs = ["string_writer.h"],
    srcs = ["string_writer.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":writer",
        "//jasmin/core:function",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "string_writer_test",
    srcs = ["string_writer_test.cc"],
    deps = [
        ":string_writer",
        "@asoffer_nth//nth/test:main",
    ],
)

cc_library(
    name = "writer",
    hdrs = ["writer.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "writer_test",
    srcs = ["writer_test.cc"],
    deps = [
        ":string_writer",
        ":writer",
        "@asoffer_nth//nth/test:main",
    ],
)
