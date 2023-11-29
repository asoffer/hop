package(default_visibility = ["//visibility:private"])

cc_library(
    name = "op_code",
    hdrs = ["op_code.h"],
    deps = [
        "//jasmin:instruction",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/meta:sequence",
        "@asoffer_nth//nth/meta:type",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_library(
    name = "ssa",
    hdrs = ["ssa.h"],
    srcs = ["ssa.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":op_code",
        "//jasmin:instruction",
        "//jasmin:function",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/strings:interpolate",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_binary(
    name = "test",
    srcs = ["main.cc"],
    deps = [
        ":ssa",
        "//jasmin/instructions:arithmetic",
        "//jasmin/instructions:compare",
        "//jasmin/instructions:core",
        "@asoffer_nth//nth/debug",
        "@asoffer_nth//nth/debug/log:stderr_log_sink",
    ],
)
