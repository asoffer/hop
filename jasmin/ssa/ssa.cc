#include "jasmin/ssa/ssa.h"

#include "absl/synchronization/mutex.h"

namespace jasmin {
namespace {

enum {
  BuiltinCall = 0,
  BuiltinJump,
  BuiltinJumpIf,
  BuiltinReturn,
};

std::vector<uint64_t> BlockBoundaries(
    OpCodeMetadata (*decode)(Value), std::span<Value const> instructions,
    std::span<internal::exec_fn_type const> builtins) {
  std::vector<uint64_t> block_boundaries{0};
  Value const* start = instructions.data();
  Value const* p     = start;
  while (p != instructions.data() + instructions.size()) {
    auto metadata = decode(*p);
    if (p->as<internal::exec_fn_type>() == builtins[BuiltinJump] or
        p->as<internal::exec_fn_type>() == builtins[BuiltinJumpIf]) {
      block_boundaries.push_back(p - start + 2);
      block_boundaries.push_back((p - start) + (p + 1)->as<ptrdiff_t>());
    }
    p += metadata.immediate_value_count + 1;
  }

  block_boundaries.push_back(instructions.size());
  std::sort(block_boundaries.begin(), block_boundaries.end());
  auto iter = std::unique(block_boundaries.begin(), block_boundaries.end());
  block_boundaries.erase(iter, block_boundaries.end());
  return block_boundaries;
}

struct BasicBlockRegisterStack {
  explicit BasicBlockRegisterStack(std::vector<SsaValue>& registers,
                                   size_t& first)
      : next_(first), registers_(registers) {}

  std::vector<SsaValue> AssignCall(InstructionSpecification spec) {
    std::vector<SsaValue> parameters;
    parameters.push_back(registers_.back());
    registers_.pop_back();
    parameters.insert(parameters.end(), registers_.end() - spec.parameters,
                      registers_.end());
    registers_.resize(registers_.size() - spec.parameters, SsaRegister());
    for (size_t i = 0; i < spec.returns; ++i) {
      SsaValue r = NewRegister();
      registers_.push_back(r);
      parameters.push_back(r);
    }

    return parameters;
  }

  std::vector<SsaValue> Assign(std::span<Value const> immediates,
                               OpCodeMetadata const& metadata) {
    EnsureStackSize(metadata.parameter_count);

    std::vector<SsaValue> parameters;
    for (Value v : immediates) { parameters.push_back(SsaValue::Immediate(v)); }
    parameters.insert(parameters.end(),
                      registers_.end() - metadata.parameter_count,
                      registers_.end());

    if (metadata.consumes_input) {
      registers_.resize(registers_.size() - metadata.parameter_count,
                        SsaRegister());
    } else {
      for (auto iter = registers_.end() - metadata.parameter_count;
           iter != registers_.end(); ++iter) {
        *iter = NewRegister();
        parameters.push_back(*iter);
      }
    }

    for (size_t i = 0; i < metadata.return_count; ++i) {
      SsaValue r = NewRegister();
      registers_.push_back(r);
      parameters.push_back(r);
    }

    return parameters;
  }

  std::vector<SsaValue>&& BlockParameters() && {
    std::reverse(block_parameters_.begin(), block_parameters_.end());
    return std::move(block_parameters_);
  }

  void EnsureStackSize(size_t parameter_count) {
    if (registers_.size() < parameter_count) {
      std::vector<SsaValue> r;
      for (size_t i = registers_.size(); i < parameter_count; ++i) {
        SsaValue reg = NewRegister();
        block_parameters_.push_back(reg);
        r.push_back(reg);
      }
      r.insert(r.end(), registers_.begin(), registers_.end());
      registers_ = std::move(r);
    }
  }

 private:
  constexpr SsaRegister NewRegister() { return SsaRegister(next_++); }

  size_t& next_;
  std::vector<SsaValue> block_parameters_;
  std::vector<SsaValue>& registers_;
};

struct StackToSsaConverter {
  explicit StackToSsaConverter(OpCodeMetadata (*decode)(Value), size_t returns,
                               std::span<internal::exec_fn_type const> builtins)
      : decode_(decode), returns_(returns), builtins_(builtins) {}

  std::vector<SsaValue> ConvertBasicBlock(SsaBasicBlock& block,
                                          std::span<Value const> instructions) {
    std::vector<SsaValue> registers;
    BasicBlockRegisterStack bb_reg_stack(registers, register_count_);
    while (not instructions.empty()) {
      auto inst = instructions.front().as<internal::exec_fn_type>();

      uint64_t immediate_bits;
      uint64_t output_count;
      std::vector<SsaValue> parameters;
      if (inst == builtins_[BuiltinCall]) {
        parameters =
            bb_reg_stack.AssignCall(instructions[1].as<InstructionSpecification>());
        instructions   = instructions.subspan(2);
        immediate_bits = 0;
        output_count   = instructions[1].as<InstructionSpecification>().returns;
      } else if (inst == builtins_[BuiltinReturn]) {
        bb_reg_stack.EnsureStackSize(returns_);
        immediate_bits = 0;
        output_count   = 0;
        instructions   = instructions.subspan(1);
      } else {
        auto metadata = decode_(inst);
        parameters    = bb_reg_stack.Assign(
               instructions.subspan(1, metadata.immediate_value_count), metadata);
        instructions = instructions.subspan(metadata.immediate_value_count + 1);
        immediate_bits = (uint64_t{1} << metadata.immediate_value_count) - 1;
        output_count =
            (not metadata.consumes_input) * metadata.parameter_count +
            metadata.return_count;
      }
      block.append(SsaInstruction(inst, output_count, immediate_bits,
                                  std::move(parameters)));
    }
    block.set_parameters(std::move(bb_reg_stack).BlockParameters());
    return registers;
  }

 private:
  size_t register_count_ = 0;
  OpCodeMetadata (*decode_)(Value);
  size_t returns_;
  std::span<internal::exec_fn_type const> builtins_;
};

absl::Mutex name_decoder_mutex;
absl::flat_hash_map<internal::exec_fn_type, std::string> name_decoder;

}  // namespace

namespace internal {

void InsertNameDecodings(
    std::span<std::pair<exec_fn_type, std::string_view> const> pairs) {
  absl::MutexLock lock(&name_decoder_mutex);
  for (auto const& [f, name] : pairs) { name_decoder.emplace(f, name); }
}

std::string InstructionNameDecoder(internal::exec_fn_type fn) {
  absl::MutexLock lock(&name_decoder_mutex);
  auto iter = name_decoder.find(fn);
  if (iter == name_decoder.end()) { return "???"; }
  return iter->second;
}

}  // namespace internal

void SsaFunction::Initialize(OpCodeMetadata (*decode)(Value),
                             std::span<Value const> instructions,
                             std::span<internal::exec_fn_type const> builtins) {
  std::vector<uint64_t> block_boundaries =
      BlockBoundaries(decode, instructions, builtins);
  blocks_.resize(block_boundaries.size() - 1);

  size_t register_count = parameter_count_;
  std::vector<SsaValue> registers;
  for (size_t i = 0; i < register_count; ++i) {
    registers.push_back(SsaRegister(i));
  }

  std::vector<std::vector<SsaValue>> registers_on_exit;
  StackToSsaConverter converter(decode, return_count_, builtins);
  auto block_iter = blocks_.begin();
  for (auto b = block_boundaries.begin(); b + 1 != block_boundaries.end();
       ++b, ++block_iter) {
    registers_on_exit.push_back(converter.ConvertBasicBlock(
        *block_iter, std::span<Value const>(instructions.data() + *b,
                                            instructions.data() + *(b + 1))));
  }

  for (size_t i = 0; i < blocks_.size(); ++i) {
    auto& block = blocks_[i];
    if (block.instructions().empty()) {
      size_t size    = blocks_[i + 1].parameters().size();
      std::span span = registers_on_exit[i];
      block.set_branch(SsaBranch::Unconditional(
          i + 1, span.subspan(span.size() - size, size)));
    } else if (block.instructions().back().op_code() == builtins[BuiltinJump]) {
      auto const& inst = block.instructions().back();
      auto boundary_iter =
          std::lower_bound(block_boundaries.begin(), block_boundaries.end(),
                           inst.argument_value(0).immediate().as<ptrdiff_t>());
      auto target    = std::distance(block_boundaries.begin(), boundary_iter);
      size_t size    = blocks_[target].parameters().size();
      std::span span = registers_on_exit[i];
      block.set_branch(SsaBranch::Unconditional(
          target, span.subspan(span.size() - size, size)));
      block.remove_back();
    } else if (block.instructions().back().op_code() ==
               builtins[BuiltinJumpIf]) {
      auto const& inst = block.instructions().back();
      auto boundary_iter =
          std::lower_bound(block_boundaries.begin(), block_boundaries.end(),
                           inst.argument_value(0).immediate().as<ptrdiff_t>());
      auto true_block  = std::distance(block_boundaries.begin(), boundary_iter);
      auto false_block = i + 1;
      size_t true_size = blocks_[true_block].parameters().size();
      size_t false_size = blocks_[false_block].parameters().size();
      std::span span    = registers_on_exit[i];
      block.set_branch(SsaBranch::Conditional(
          inst.argument_value(1), true_block,
          span.subspan(span.size() - true_size, true_size), false_block,
          span.subspan(span.size() - false_size, false_size)));
      block.remove_back();
    } else if (block.instructions().back().op_code() ==
               builtins[BuiltinReturn]) {
      block.remove_back();
      std::span span = registers_on_exit[i];
      block.set_branch(SsaBranch::Return(
          span.subspan(span.size() - return_count_, return_count_)));
    } else {
      NTH_REQUIRE((v.harden), not registers_on_exit.empty());
      size_t size    = blocks_[i + 1].parameters().size();
      std::span span = registers_on_exit[i];
      block.set_branch(SsaBranch::Unconditional(
          i + 1, span.subspan(span.size() - size, size)));
    }
  }
}

}  // namespace jasmin
