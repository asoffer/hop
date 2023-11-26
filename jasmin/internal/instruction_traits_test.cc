#include "jasmin/internal/instruction_traits.h"

#include "jasmin/instruction.h"
#include "nth/test/test.h"

namespace jasmin::internal {
namespace {

struct Nop {
  static void execute(std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<Nop>);

struct NopConsume {
  static void consume(std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<NopConsume>);

struct FunctionStateNop {
  using function_state = int;
  static void execute(function_state&, std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<FunctionStateNop>);

struct FunctionStateNopConsume {
  using function_state = int;
  static void consume(function_state&, std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<FunctionStateNopConsume>);

struct Returns {
  static bool execute(std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<Returns>);

struct ReturnsConsume {
  static bool consume(std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<ReturnsConsume>);

struct FunctionStateReturns {
  using function_state = int;
  static bool execute(function_state&, std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<FunctionStateReturns>);

struct FunctionStateReturnsConsume {
  using function_state = int;
  static bool consume(function_state&, std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsConsume>);

struct ReturnsAndAcceptsValues {
  static bool execute(std::span<Value, 2>);
};
static_assert(UserDefinedInstruction<ReturnsAndAcceptsValues>);

struct ReturnsAndAcceptsValuesConsume {
  static bool consume(std::span<Value, 2>);
};
static_assert(UserDefinedInstruction<ReturnsAndAcceptsValuesConsume>);

struct FunctionStateReturnsAndAcceptsValues {
  using function_state = int;
  static bool execute(function_state&, std::span<Value, 2>);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsAndAcceptsValues>);

struct FunctionStateReturnsAndAcceptsValuesConsume {
  using function_state = int;
  static bool consume(function_state&, std::span<Value, 2>);
};
static_assert(
    UserDefinedInstruction<FunctionStateReturnsAndAcceptsValuesConsume>);

struct ReturnsAndAcceptsValuesAndImmediates {
  static bool execute(std::span<Value, 2>, int, char);
};
static_assert(UserDefinedInstruction<ReturnsAndAcceptsValuesAndImmediates>);

struct ReturnsAndAcceptsValuesAndImmediatesConsume {
  static bool consume(std::span<Value, 2>, int, char);
};
static_assert(
    UserDefinedInstruction<ReturnsAndAcceptsValuesAndImmediatesConsume>);

struct FunctionStateReturnsAndAcceptsValuesAndImmediates {
  using function_state = int;
  static bool execute(function_state&, std::span<Value, 2>, int, char);
};
static_assert(
    UserDefinedInstruction<FunctionStateReturnsAndAcceptsValuesAndImmediates>);

struct FunctionStateReturnsAndAcceptsValuesAndImmediatesConsume {
  using function_state = int;
  static bool consume(function_state&, std::span<Value, 2>, int, char);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsAndAcceptsValuesAndImmediatesConsume>);

}  // namespace
}  // namespace jasmin::internal
