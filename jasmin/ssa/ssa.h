#ifndef JASMIN_SSA_H
#define JASMIN_SSA_H

#include <cstdint>
#include <string_view>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "jasmin/core/function.h"
#include "jasmin/core/instruction.h"
#include "jasmin/core/metadata.h"
#include "jasmin/core/value.h"
#include "nth/container/disjoint_set.h"
#include "nth/debug/debug.h"

namespace jasmin {
namespace internal {

template <InstructionSetType Set>
constexpr std::array BuiltinPointers{
    &Call::ExecuteImpl<Set>, &Jump::ExecuteImpl<Set>, &JumpIf::ExecuteImpl<Set>,
    &JumpIfNot::ExecuteImpl<Set>, &Return::ExecuteImpl<Set>};

void InsertNameDecodings(
    std::span<std::pair<exec_fn_type, std::string_view> const> pairs);
std::string InstructionNameDecoder(internal::exec_fn_type fn);

}  // namespace internal

struct SsaRegister {
  explicit constexpr SsaRegister(
      uint64_t n = std::numeric_limits<uint64_t>::max())
      : number_(n) {}

  friend constexpr bool operator==(SsaRegister, SsaRegister) = default;
  constexpr uint64_t value() const { return number_; }

 private:
  uint64_t number_;
};

namespace internal {
union SsaValueImpl {
  Value immediate_;
  SsaRegister reg_;
};
}  // namespace internal

struct SsaValue {
  constexpr SsaValue(SsaRegister r) : impl_(r), is_reg_(true) {}
  static SsaValue Immediate(Value v) {
    SsaValue val;
    val.is_reg_          = false;
    val.impl_.immediate_ = v;
    return val;
  }

  constexpr Value immediate() const { return impl_.immediate_; }
  constexpr SsaRegister reg() const { return impl_.reg_; }
  constexpr bool is_register() const { return is_reg_; }

  template <typename H>
  friend H AbslHashValue(H h, SsaValue const &v) {
    if (v.is_reg_) {
      return H::combine(std::move(h), v.impl_.reg_.value());
    } else {
      return H::combine(std::move(h), v.impl_.immediate_.raw_value());
    }
  }

  friend bool operator==(SsaValue const &lhs, SsaValue const &rhs) {
    if (lhs.is_reg_ != rhs.is_reg_) { return false; }
    if (lhs.is_reg_) {
      return lhs.impl_.reg_ == rhs.impl_.reg_;
    } else {
      return lhs.impl_.immediate_.raw_value() ==
             rhs.impl_.immediate_.raw_value();
    }
  }

  friend bool operator!=(SsaValue const &lhs, SsaValue const &rhs) {
    return not(lhs == rhs);
  }

 private:
  constexpr SsaValue() : impl_(SsaRegister()) {}

  internal::SsaValueImpl impl_;
  bool is_reg_;
};

struct SsaInstruction {
  explicit SsaInstruction(internal::exec_fn_type op_code, uint64_t output_count,
                          std::vector<SsaValue> values)
      : op_code_(op_code),
        output_count_(output_count),
        arguments_(std::move(values)) {}

  constexpr internal::exec_fn_type op_code() const { return op_code_; }

  std::span<SsaValue> arguments() {
    return std::span<SsaValue>(arguments_.begin(),
                               arguments_.end() - output_count_);
  }

  std::span<SsaValue const> arguments() const {
    return std::span<SsaValue const>(arguments_.begin(),
                                     arguments_.end() - output_count_);
  }
  std::span<SsaValue> outputs() {
    return std::span<SsaValue>(arguments_.end() - output_count_,
                               arguments_.end());
  }

  std::span<SsaValue const> outputs() const {
    return std::span<SsaValue const>(arguments_.end() - output_count_,
                                     arguments_.end());
  }

  SsaValue &argument(size_t i) { return arguments_[i]; }
  SsaValue const &argument(size_t i) const { return arguments_[i]; }
  SsaValue &output(size_t i) { return *(arguments_.end() - output_count_ + i); }
  SsaValue const &output(size_t i) const {
    return *(arguments_.end() - output_count_ + i);
  }

 private:
  internal::exec_fn_type op_code_;
  uint64_t output_count_;
  std::vector<SsaValue> arguments_;
};

enum class SsaBranchKind { Unreachable, Unconditional, Conditional, Return };

struct SsaBranch {
  struct ConditionalImpl {
    SsaValue value;
    size_t true_block;
    size_t false_block;
    size_t true_false_split;
    std::vector<SsaValue> block_arguments;

    std::span<SsaValue const> true_arguments() const {
      return std::span(block_arguments).subspan(0, true_false_split);
    }

    std::span<SsaValue const> false_arguments() const {
      return std::span(block_arguments).subspan(true_false_split);
    }
  };

  SsaBranch() : SsaBranch(UnreachableImpl{}) {}

  SsaBranchKind kind() const {
    return static_cast<SsaBranchKind>(branch_.index());
  }

  ConditionalImpl const &AsConditional() const {
    return std::get<ConditionalImpl>(branch_);
  }

  static SsaBranch Unreachable() { return SsaBranch(UnreachableImpl{}); }
  static SsaBranch Unconditional(size_t block, std::span<SsaValue const> args) {
    return SsaBranch(UnconditionalImpl{
        .block           = block,
        .block_arguments = std::vector(args.begin(), args.end())});
  }
  static SsaBranch Conditional(SsaValue value, size_t true_block,
                               std::span<SsaValue const> true_args,
                               size_t false_block,
                               std::span<SsaValue const> false_args) {
    if (value.is_register()) {
      std::vector<SsaValue> args(true_args.begin(), true_args.end());
      args.insert(args.end(), false_args.begin(), false_args.end());
      return SsaBranch(ConditionalImpl{.value            = value,
                                       .true_block       = true_block,
                                       .false_block      = false_block,
                                       .true_false_split = true_args.size(),
                                       .block_arguments  = std::move(args)});
    } else {
      if (value.immediate().as<bool>()) {
        return SsaBranch(UnconditionalImpl{
            .block = true_block,
            .block_arguments =
                std::vector(true_args.begin(), true_args.end())});
      } else {
        return SsaBranch(UnconditionalImpl{
            .block = false_block,
            .block_arguments =
                std::vector(false_args.begin(), false_args.end())});
      }
    }
  }
  static SsaBranch Return(std::span<SsaValue const> arguments) {
    return SsaBranch(ReturnImpl{
        .block_arguments = std::vector(arguments.begin(), arguments.end()),
    });
  }

  void rename(nth::disjoint_set<SsaValue> &);

 private:
  struct UnreachableImpl {};
  struct UnconditionalImpl {
    size_t block;
    std::vector<SsaValue> block_arguments;
  };
  struct ReturnImpl {
    std::vector<SsaValue> block_arguments;
  };
  using variant_type = std::variant<UnreachableImpl, UnconditionalImpl,
                                    ConditionalImpl, ReturnImpl>;

  explicit SsaBranch(variant_type branch) : branch_(std::move(branch)) {}

  variant_type branch_;
};

struct SsaBasicBlock {
  void append(SsaInstruction i) { instructions_.push_back(std::move(i)); }
  void remove_back() { instructions_.pop_back(); }

  void set_parameters(std::vector<SsaValue> parameters) {
    parameters_ = std::move(parameters);
  }
  void set_branch(SsaBranch branch) { branch_ = std::move(branch); }
  SsaBranch const &branch() const { return branch_; }

  constexpr std::span<SsaValue const> parameters() const { return parameters_; }
  constexpr std::span<SsaValue> parameters() { return parameters_; }

  std::span<SsaInstruction const> instructions() const { return instructions_; }
  std::span<SsaInstruction> instructions() { return instructions_; }

 private:
  std::vector<SsaValue> parameters_;
  std::vector<SsaInstruction> instructions_;
  SsaBranch branch_;
};

struct SsaFunction {
  template <int &..., InstructionSetType Set>
  SsaFunction(Function<Set> const &f)
      : parameter_count_(f.parameter_count()), return_count_(f.return_count()) {
    InitializeNameDecoder<Set>();
    Initialize(
        [](Value fn) -> decltype(auto) {
          return Metadata<Set>().metadata(Metadata<Set>().opcode(fn));
        },
        f.raw_instructions(), internal::BuiltinPointers<Set>);
  }

  std::span<SsaBasicBlock const> blocks() const { return blocks_; }
  std::span<SsaBasicBlock> blocks() { return blocks_; }

 private:
  void Initialize(InstructionMetadata const &(*decode)(Value),
                  std::span<Value const> instructions,
                  std::span<internal::exec_fn_type const> builtins);

  template <InstructionSetType Set>
  static void InitializeNameDecoder() {
    [[maybe_unused]] static bool const initializer =
        Set::instructions.reduce([](auto... ts) {
          InsertNameDecodings(
              std::array{std::pair(&nth::type_t<ts>::template ExecuteImpl<Set>,
                                   std::string_view(ts.name()))...});
          return true;
        });
  }

  uint8_t parameter_count_;
  uint8_t return_count_;
  std::vector<SsaBasicBlock> blocks_;
};

}  // namespace jasmin

#endif  // JASMIN_SSA_H
