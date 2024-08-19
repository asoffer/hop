package(default_visibility = ["//visibility:private"])

cc_library(
    name = "register_coalescer",
    hdrs = ["register_coalescer.h"],
    srcs = ["register_coalescer.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":ssa",
        "@nth_cc//nth/debug",
        "@nth_cc//nth/container:disjoint_set",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_library(
    name = "ssa",
    hdrs = ["ssa.h"],
    srcs = ["ssa.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//hop/core:function",
        "//hop/core:instruction",
        "//hop/core:metadata",
        "@nth_cc//nth/debug",
        "@nth_cc//nth/container:disjoint_set",
        "@com_google_absl//absl/synchronization",
    ],
)
