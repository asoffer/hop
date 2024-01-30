#include "jasmin/compile/x64/code_generator.h"

namespace jasmin::x64 {

void CodeGenerator::write(std::initializer_list<uint8_t> instructions) {
  fn_->write(instructions);
}

void CodeGenerator::push(Register reg) {
  write({static_cast<uint8_t>(static_cast<uint8_t>(reg) + 0x50)});
}

void CodeGenerator::pop(Register reg) {
  write({static_cast<uint8_t>(static_cast<uint8_t>(reg) + 0x58)});
}

void CodeGenerator::mov(Register destination, Register source) {
  write({0x48, 0x89,
         static_cast<uint8_t>(0xc0 + static_cast<uint8_t>(destination) +
                              8 * static_cast<uint8_t>(source))});
}

void CodeGenerator::syscall() { write({0x0f, 0x05}); }

void CodeGenerator::ret() { write({0xc3}); }

void CodeGenerator::function(SsaFunction const &fn, CompiledFunction &c) {
  fn_ = &c;
  push(Register::rbp);
  mov(Register::rbp, Register::rsp);

  block_starts_.reserve(fn.blocks().size());
  LocationMap loc_map;
  for (auto const &block : fn.blocks()) {
    block_starts_.push_back(fn_->size());
    for (auto const &inst : block.instructions()) {
      generators_[metadata_.opcode(inst.op_code())](*this, loc_map);
    }
    switch (block.branch().kind()) {
      case SsaBranchKind::Return:
        mov(Register::rsp, Register::rbp);
        pop(Register::rbp);
        ret();
        break;
      case SsaBranchKind::Conditional: {
        auto const &c = block.branch().AsConditional();
        // TODO: Support arbirtary register choices here.
        // TODO: Prefer fallthroughs when we can make that happen.
        write({
            0x84, 0xc0,                          // test al, al
            0x0f, 0x84, 0x00, 0x00, 0x00, 0x00,  // jz __
        });
        block_jumps_.emplace(fn_->size(), c.true_block);
        write({
            0xe9, 0x00, 0x00, 0x00, 0x00,  // jmp ___
        });
        block_jumps_.emplace(fn_->size(), c.false_block);
      } break;
      case SsaBranchKind::Unconditional: {
        NTH_UNIMPLEMENTED();
      } break;
      case SsaBranchKind::Unreachable: break;
    }
  }

  for (auto const & [offset, block_number] : block_jumps_) {
    fn_->write_at(offset - 4,
                  static_cast<uint32_t>(block_starts_[block_number] - offset));
  }

  fn_ = nullptr;
}

}  // namespace jasmin
