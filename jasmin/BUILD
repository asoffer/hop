package(default_visibility = ["//visibility:private"])

cc_library(
    name = "call_stack",
    hdrs = ["call_stack.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/internal:function_base",
    ],
)

cc_library(
    name = "function",
    hdrs = ["function.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":call_stack",
        "//jasmin/internal:function_base",
    ],
)

cc_library(
    name = "instruction",
    hdrs = ["instruction.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":call_stack",
        ":instruction_pointer",
        ":value",
        ":value_stack",
        "//jasmin/internal:attributes",
        "//jasmin/internal:function_base",
        "//jasmin/internal:type_traits",
    ],
)

cc_library(
    name = "instruction_pointer",
    hdrs = ["instruction_pointer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "//jasmin/internal:debug",
    ],
)


cc_library(
    name = "value_stack",
    hdrs = ["value_stack.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":value",
        "//jasmin/internal:debug",
    ],
)

cc_library(
    name = "value",
    hdrs = ["value.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//jasmin/internal:debug",
    ],
)
