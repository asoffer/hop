#include "jasmin/debug.h"

#include "gtest/gtest.h"
#include "jasmin/instructions/core.h"
#include "jasmin/testing.h"

namespace jasmin {

TEST(ShowValue, ShowValue) {
  EXPECT_EQ(ShowValue(Value(int32_t{5})),
#if defined(JASMIN_DEBUG)
            "5 (int32_t)"
#else
            "05 00 00 00 00 00 00 00"
#endif
  );
  EXPECT_EQ(ShowValue(Value(int64_t{-257})),
#if defined(JASMIN_DEBUG)
            "-257 (int64_t)"
#else
            "ff fe ff ff ff ff ff ff"
#endif
  );

  void* ptr = reinterpret_cast<void*>(0x12345678);
  EXPECT_EQ(ShowValue(Value(ptr)),
#if defined(JASMIN_DEBUG)
            "addr(0x0000000012345678)"
#else
            "78 56 34 12 00 00 00 00"
#endif
  );
}

TEST(InstructionName, InstructionName) {
  EXPECT_EQ(InstructionName<Store>(), "store");
  EXPECT_EQ(InstructionName<Drop>(), "drop");
}

TEST(ShowValueStack, ShowValueStack) {
  EXPECT_EQ(ShowValueStack({}), "");
  EXPECT_EQ(ShowValueStack({int16_t{1}}),
#if defined(JASMIN_DEBUG)
            "[1 (int16_t)]\n"
#else
            "[01 00 00 00 00 00 00 00]\n"
#endif
  );
  EXPECT_EQ(ShowValueStack({int64_t{10}, true, uint32_t{257}, float{1.1}}),
#if defined(JASMIN_DEBUG)
            "[10 (int64_t)]\n"
            "[true]\n"
            "[257 (uint32_t)]\n"
            "[1.100000 (float)]\n"
#else
            "[0a 00 00 00 00 00 00 00]\n"
            "[01 00 00 00 00 00 00 00]\n"
            "[01 01 00 00 00 00 00 00]\n"
            "[cd cc 8c 3f 00 00 00 00]\n"
#endif
  );
}

TEST(DumpValueStack, DumpValueStack) {
  static std::string output;
  ValueStack value_stack;
  ExecuteInstruction<DumpValueStack>(
      value_stack, [](std::string_view s) { output = std::string(s); });
  EXPECT_EQ(output, "");

  value_stack = {int64_t{10}, true, uint32_t{257}, float{1.1}};
  ExecuteInstruction<DumpValueStack>(
      value_stack, [](std::string_view s) { output = std::string(s); });
  EXPECT_EQ(output,
#if defined(JASMIN_DEBUG)
            "[10 (int64_t)]\n"
            "[true]\n"
            "[257 (uint32_t)]\n"
            "[1.100000 (float)]\n"
#else
            "[0a 00 00 00 00 00 00 00]\n"
            "[01 00 00 00 00 00 00 00]\n"
            "[01 01 00 00 00 00 00 00]\n"
            "[cd cc 8c 3f 00 00 00 00]\n"
#endif
  );
}

struct AddNoImmediates : StackMachineInstruction<AddNoImmediates> {
  static std::string_view debug(std::span<Value const, 0>) { return "add"; }
  static int execute(int x, int y, int z) { return x + y + z; }
};

struct AddImmediates : StackMachineInstruction<AddImmediates> {
  static std::string debug(std::span<Value const, 3> immediate_values) {
    std::string result = "add";
    for (Value v : immediate_values) {
      result += " ";
      result += std::to_string(v.as<int>());
    }
    return result;
  }
  static void execute(ValueStack&, int, int, int) {}
};


TEST(DumpInstruction, DumpInstruction) {
  Value vs[1] = {ptrdiff_t{5}};
  EXPECT_EQ(DumpInstruction<Call>({}), "call");
  EXPECT_EQ(DumpInstruction<Return>({}), "return");
  EXPECT_EQ(DumpInstruction<Jump>(vs), "jump +5");
  EXPECT_EQ(DumpInstruction<JumpIf>(vs), "jump-if +5");

  Value operands[3] = {1, 4, 9};
  EXPECT_EQ(DumpInstruction<AddNoImmediates>({}), "add");
  EXPECT_EQ(DumpInstruction<AddImmediates>(operands), "add 1 4 9");
}


TEST(DumpFunction, DumpFunction) {
  using Set = MakeInstructionSet<AddNoImmediates, AddImmediates>;
  using Fn = Function<Set>;

  Fn f(0, 0);
  f.append<Call>();
  f.append<AddImmediates>(34, 55, 89);
  f.append<AddNoImmediates>();
  f.append<Return>();

  EXPECT_EQ(DumpFunction(f),
            "call\n"
            "add 34 55 89\n"
            "add\n"
            "return\n");
}

}  // namespace jasmin
