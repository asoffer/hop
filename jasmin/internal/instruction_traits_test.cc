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

struct ReturnsMultiple {
  static std::array<Value, 3> execute(std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<ReturnsMultiple>);

struct ReturnsMultipleConsume {
  static std::array<Value, 3> consume(std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<ReturnsMultipleConsume>);

struct FunctionStateReturnsMultiple {
  using function_state = int;
  static std::array<Value, 3> execute(function_state&, std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsMultiple>);

struct FunctionStateReturnsMultipleConsume {
  using function_state = int;
  static std::array<Value, 3> consume(function_state&, std::span<Value, 0>);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsMultipleConsume>);

struct ReturnsSingletonArray {
  static std::array<Value, 1> execute(std::span<Value, 0>);
};
static_assert(not UserDefinedInstruction<ReturnsSingletonArray>);

struct ReturnsSingletonArrayConsume {
  static std::array<Value, 1> consume(std::span<Value, 0>);
};
static_assert(not UserDefinedInstruction<ReturnsSingletonArrayConsume>);

struct FunctionStateReturnsSingletonArray {
  using function_state = int;
  static std::array<Value, 1> execute(function_state&, std::span<Value, 0>);
};
static_assert(not UserDefinedInstruction<FunctionStateReturnsSingletonArray>);

struct FunctionStateReturnsSingletonArrayConsume {
  using function_state = int;
  static std::array<Value, 1> consume(function_state&, std::span<Value, 0>);
};
static_assert(
    not UserDefinedInstruction<FunctionStateReturnsSingletonArrayConsume>);

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

struct TooBig {
  Value v[2];
};

struct ReturnsNotConvertibleAndAcceptsValuesAndImmediates {
  static TooBig execute(std::span<Value, 2>, int, char);
};
static_assert(not UserDefinedInstruction<
              ReturnsNotConvertibleAndAcceptsValuesAndImmediates>);

struct ReturnsNotConvertibleAndAcceptsValuesAndImmediatesConsume {
  static TooBig consume(std::span<Value, 2>, int, char);
};
static_assert(not UserDefinedInstruction<
              ReturnsNotConvertibleAndAcceptsValuesAndImmediatesConsume>);

struct FunctionStateReturnsNotConvertibleAndAcceptsValuesAndImmediates {
  using function_state = int;
  static TooBig execute(function_state&, std::span<Value, 2>, int, char);
};
static_assert(not UserDefinedInstruction<
              FunctionStateReturnsNotConvertibleAndAcceptsValuesAndImmediates>);

struct FunctionStateReturnsNotConvertibleAndAcceptsValuesAndImmediatesConsume {
  using function_state = int;
  static TooBig consume(function_state&, std::span<Value, 2>, int, char);
};
static_assert(
    not UserDefinedInstruction<
        FunctionStateReturnsNotConvertibleAndAcceptsValuesAndImmediatesConsume>);

struct ExecuteDynamic : Instruction<ExecuteDynamic> {
  static void execute(std::span<Value>, std::span<Value>);
};
static_assert(UserDefinedInstruction<ExecuteDynamic>);

struct ConsumeDynamic : Instruction<ConsumeDynamic> {
  static void consume(std::span<Value>, std::span<Value>);
};
static_assert(UserDefinedInstruction<ConsumeDynamic>);

struct DynamicMustReturnVoid : Instruction<DynamicMustReturnVoid> {
  static bool consume(std::span<Value>, std::span<Value>);
};
static_assert(not UserDefinedInstruction<DynamicMustReturnVoid>);

struct ExecuteDynamicWithImmediates
    : Instruction<ExecuteDynamicWithImmediates> {
  static void execute(std::span<Value>, std::span<Value>, int);
};
static_assert(UserDefinedInstruction<ExecuteDynamicWithImmediates>);

struct ConsumeDynamicWithImmediates
    : Instruction<ConsumeDynamicWithImmediates> {
  static void consume(std::span<Value>, std::span<Value>, int);
};
static_assert(UserDefinedInstruction<ConsumeDynamicWithImmediates>);

}  // namespace
}  // namespace jasmin::internal
