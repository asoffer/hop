#include <iostream>

#include "arithmetic.h"
#include "compare.h"
#include "core.h"
#include "instruction.h"

using Instructions =
    jasmin::InstructionTable<jasmin::Return, jasmin::Duplicate, jasmin::Swap,
                             jasmin::JumpIf, jasmin::Call, jasmin::Push,
                             jasmin::LessThan<uint64_t>, jasmin::Add<uint64_t>,
                             jasmin::Subtract<uint64_t>,
                             jasmin::Multiply<uint64_t>>;

int main(int argc, char const *argv[]) {
  Instructions::function_type func(1, 1);
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push>(uint64_t{2});
  func.append<jasmin::LessThan<uint64_t>>();
  auto index = func.append_conditional_jump();
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push>(uint64_t{1});
  func.append<jasmin::Subtract<uint64_t>>();
  func.append<jasmin::Push>(&func);
  func.append<jasmin::Call>();
  func.append<jasmin::Swap>();
  func.append<jasmin::Push>(uint64_t{2});
  func.append<jasmin::Subtract<uint64_t>>();
  func.append<jasmin::Push>(&func);
  func.append<jasmin::Call>();
  func.append<jasmin::Add<uint64_t>>();
  func.append<jasmin::Return>();
  func.set_jump_target(index);

  uint64_t result;
  jasmin::Execute(func, {static_cast<uint64_t>(std::atoi(argv[1]))}, result);
  std::cerr << "[[" << result << "]]\n";
  return 0;
}
