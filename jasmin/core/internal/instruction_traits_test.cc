#include "jasmin/core/internal/instruction_traits.h"

#include "jasmin/core/instruction.h"
#include "nth/test/test.h"

namespace jasmin::internal {
namespace {

struct Nop {
  static void execute(Input<>, Output<>);
};
static_assert(UserDefinedInstruction<Nop>);

struct NopConsume {
  static void consume(Input<>, Output<>);
};
static_assert(UserDefinedInstruction<NopConsume>);

struct FunctionStateNop {
  using function_state = int;
  static void execute(function_state&, Input<>, Output<>);
};
static_assert(UserDefinedInstruction<FunctionStateNop>);

struct FunctionStateNopConsume {
  using function_state = int;
  static void consume(function_state&, Input<>, Output<>);
};
static_assert(UserDefinedInstruction<FunctionStateNopConsume>);

struct Returns {
  static void execute(Input<>, Output<Value>);
};
static_assert(UserDefinedInstruction<Returns>);

struct ReturnsConsume {
  static void consume(Input<>, Output<Value>);
};
static_assert(UserDefinedInstruction<ReturnsConsume>);

struct FunctionStateReturns {
  using function_state = int;
  static void execute(function_state&, Input<>, Output<Value>);
};
static_assert(UserDefinedInstruction<FunctionStateReturns>);

struct FunctionStateReturnsConsume {
  using function_state = int;
  static void consume(function_state&, Input<>, Output<Value>);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsConsume>);

struct ReturnsMultiple {
  static void execute(Input<>, Output<Value, Value, Value>);
};
static_assert(UserDefinedInstruction<ReturnsMultiple>);

struct ReturnsMultipleConsume {
  static void consume(Input<>, Output<Value, Value, Value>);
};
static_assert(UserDefinedInstruction<ReturnsMultipleConsume>);

struct FunctionStateReturnsMultiple {
  using function_state = int;
  static void execute(function_state&, Input<>, Output<Value, Value, Value>);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsMultiple>);

struct FunctionStateReturnsMultipleConsume {
  using function_state = int;
  static void consume(function_state&, Input<>, Output<Value, Value, Value>);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsMultipleConsume>);

struct ReturnsAndAcceptsValues {
  static void execute(Input<Value, Value>, Output<Value>);
};
static_assert(UserDefinedInstruction<ReturnsAndAcceptsValues>);

struct ReturnsAndAcceptsValuesConsume {
  static void consume(Input<Value, Value>, Output<Value>);
};
static_assert(UserDefinedInstruction<ReturnsAndAcceptsValuesConsume>);

struct FunctionStateReturnsAndAcceptsValues {
  using function_state = int;
  static void execute(function_state&, Input<Value, Value>, Output<Value>);
};
static_assert(UserDefinedInstruction<FunctionStateReturnsAndAcceptsValues>);

struct FunctionStateReturnsAndAcceptsValuesConsume {
  using function_state = int;
  static void consume(function_state&, Input<Value, Value>, Output<Value>);
};
static_assert(
    UserDefinedInstruction<FunctionStateReturnsAndAcceptsValuesConsume>);

struct ReturnsAndAcceptsValuesAndImmediates {
  static void execute(Input<Value, Value>, Output<Value>, int, char);
};
static_assert(UserDefinedInstruction<ReturnsAndAcceptsValuesAndImmediates>);

struct ReturnsAndAcceptsValuesAndImmediatesConsume {
  static void consume(Input<Value, Value>, Output<Value>, int, char);
};
static_assert(
    UserDefinedInstruction<ReturnsAndAcceptsValuesAndImmediatesConsume>);

struct FunctionStateReturnsAndAcceptsValuesAndImmediates {
  using function_state = int;
  static void execute(function_state&, Input<Value, Value>, Output<Value>, int,
                      char);
};
static_assert(
    UserDefinedInstruction<FunctionStateReturnsAndAcceptsValuesAndImmediates>);

struct FunctionStateReturnsAndAcceptsValuesAndImmediatesConsume {
  using function_state = int;
  static void consume(function_state&, Input<Value, Value>, Output<Value>, int,
                      char);
};
static_assert(UserDefinedInstruction<
              FunctionStateReturnsAndAcceptsValuesAndImmediatesConsume>);

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

struct BigTriviallyCopyable {
  static void execute(Input<>, Output<>, std::array<int, 20>);
};
static_assert(UserDefinedInstruction<BigTriviallyCopyable>);

struct BigTriviallyCopyableByReference {
  static void execute(Input<>, Output<>, std::array<int, 20> const &);
};
static_assert(UserDefinedInstruction<BigTriviallyCopyableByReference>);

struct BigTriviallyCopyableByMutableReference {
  static void execute(Input<>, Output<>, std::array<int, 20> &);
};
static_assert(
    not UserDefinedInstruction<BigTriviallyCopyableByMutableReference>);

}  // namespace
}  // namespace jasmin::internal
