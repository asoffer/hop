#include <iostream>

#include "arithmetic.h"
#include "compare.h"
#include "core.h"
#include "jasmin/execute.h"

using Instructions = jasmin::MakeInstructionSet<
    jasmin::Return, jasmin::Duplicate, jasmin::Swap, jasmin::JumpIf,
    jasmin::Call, jasmin::Push, jasmin::LessThan<uint64_t>,
    jasmin::Add<uint64_t>, jasmin::Subtract<uint64_t>,
    jasmin::Multiply<uint64_t>>;

int main(int argc, char const *argv[]) {
  jasmin::Function<Instructions> func(1, 1);
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push>(uint64_t{2});
  func.append<jasmin::LessThan<uint64_t>>();
  auto index = func.append_with_placeholders<jasmin::JumpIf>(1);
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
  func.set_value(index, static_cast<ptrdiff_t>(func.size() - 1));

  uint64_t result;
  jasmin::Execute(func, {static_cast<uint64_t>(std::atoi(argv[1]))}, result);
  std::cerr << "[[" << result << "]]\n";
  return 0;
}
