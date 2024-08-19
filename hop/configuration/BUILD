load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "optimize",
    hdrs = ["optimize.h"],
)

cc_library(
    name = "debug",
    hdrs = ["debug.h"],
    deps = [
      "@nth_cc//nth/meta:type",
    ],
)

cc_library(
    name = "harden",
    hdrs = ["harden.h"],
)

string_flag(
    name = "configuration",
    values = ["optimize", "debug", "harden"],
    build_setting_default = "optimize",
)

config_setting(
    name = "debug_configuration",
    flag_values = {":configuration": "debug"},
)

config_setting(
    name = "harden_configuration",
    flag_values = {":configuration": "harden"},
)

config_setting(
    name = "optimize_configuration",
    flag_values = {":configuration": "optimize"},
)

cc_library(
    name = "impl",
    hdrs = ["configuration.h"],
    visibility = ["//hop:__subpackages__"],
    deps = select({
        ":debug_configuration": [":debug"],
        ":harden_configuration": [":harden"],
        ":optimize_configuration": [":optimize"],
        "//conditions:default": [":optimize"],
    })
)
