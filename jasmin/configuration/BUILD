load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "default",
    hdrs = ["default.h"],
)

cc_library(
    name = "debug",
    hdrs = ["debug.h"],
)

string_flag(
    name = "configuration",
    values = ["default", "debug"],
    build_setting_default = "default",
)

config_setting(
    name = "debug_configuration",
    flag_values = {":configuration": "debug"},
)

cc_library(
    name = "impl",
    hdrs = ["configuration.h"],
    visibility = ["//jasmin:__subpackages__"],
    deps = select({
        ":debug_configuration": [":debug"],
        "//conditions:default": [":default"],
    })
)
