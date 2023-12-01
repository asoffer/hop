#ifndef JASMIN_SSA_H
#define JASMIN_SSA_H

#include <cstdint>
#include <string_view>
#include <vector>

#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "jasmin/ssa/op_code.h"
#include "jasmin/value.h"
#include "nth/debug/debug.h"
#include "nth/io/string_printer.h"
#include "nth/strings/interpolate.h"

namespace jasmin {
namespace internal {

template <InstructionSetType Set>
constexpr std::array BuiltinPointers{
    &Call::ExecuteImpl<Set>, &Jump::ExecuteImpl<Set>, &JumpIf::ExecuteImpl<Set>,
    &Return::ExecuteImpl<Set>};

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

  friend void NthPrint(auto &p, auto &f, SsaRegister const &r) {
    p.write("r.");
    f(p, r.value());
  }

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

  friend void NthPrint(auto &p, auto &f, SsaValue const &v) {
    if (v.is_register()) {
      f(p, v.reg());
    } else {
      p.write("imm[");
      f(p, v.immediate().raw_value());
      p.write("]");
    }
  }

 private:
  constexpr SsaValue() : impl_(SsaRegister()) {}

  internal::SsaValueImpl impl_;
  bool is_reg_;
};

struct SsaInstruction {
  explicit SsaInstruction(internal::exec_fn_type op_code, uint64_t output_count,
                          uint64_t immediate_bits, std::vector<SsaValue> values)
      : op_code_(op_code),
        output_count_(output_count),
        immediate_bits_(immediate_bits),
        arguments_(std::move(values)) {}

  constexpr internal::exec_fn_type op_code() const { return op_code_; }

  SsaValue argument_value(size_t i) const { return arguments_[i]; }

  friend void NthPrint(auto &p, auto &f, SsaInstruction const &inst) {
    p.write(" ");
    for (size_t i = inst.arguments_.size() - inst.output_count_;
         i < inst.arguments_.size(); ++i) {
      if (inst.immediate_bits_ & (uint64_t{1} << i)) {
        p.write(" imm[");
        f(p, inst.arguments_[i].immediate().raw_value());
        p.write("]");
      } else {
        p.write(" ");
        f(p, inst.arguments_[i].reg());
      }
    }
    p.write(" = ");
    f(p, internal::InstructionNameDecoder(inst.op_code_));
    for (size_t i = 0; i < inst.arguments_.size() - inst.output_count_; ++i) {
      if (inst.immediate_bits_ & (uint64_t{1} << i)) {
        p.write(" imm[");
        f(p, inst.arguments_[i].immediate().raw_value());
        p.write("]");
      } else {
        p.write(" ");
        f(p, inst.arguments_[i].reg());
      }
    }
  }

 private:
  internal::exec_fn_type op_code_;
  uint64_t output_count_;
  uint64_t immediate_bits_;
  std::vector<SsaValue> arguments_;
};

struct SsaBranch {
  SsaBranch() : SsaBranch(UnreachableImpl{}) {}

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

  friend void NthPrint(auto &p, auto &f, SsaBranch const &branch) {
    std::visit(
        [&](auto const &b) {
          constexpr auto t =
              nth::type<decltype(b)>.without_reference().without_const();
          if constexpr (t == nth::type<UnreachableImpl>) {
            p.write("  unreachable\n");
          } else if constexpr (t == nth::type<ReturnImpl>) {
            p.write("return ");
            std::string_view sep = "";
            for (SsaValue arg : b.block_arguments) {
              f(p, arg);
              p.write(std::exchange(sep, ", "));
            }
            p.write("\n");
          } else if constexpr (t == nth::type<UnconditionalImpl>) {
            p.write("  br #");
            f(p, b.block);
            p.write(" <- (");
            std::string_view sep = "";
            for (SsaValue arg : b.block_arguments) {
              f(p, arg);
              p.write(std::exchange(sep, ", "));
            }
            p.write(")\n");
          } else if constexpr (t == nth::type<ConditionalImpl>) {
            std::string true_args;
            {
              std::string_view sep = "";
              nth::string_printer sp(true_args);
              for (SsaValue arg : b.true_arguments()) {
                f(sp, arg);
                sp.write(std::exchange(sep, ", "));
              }
            }
            std::string false_args;
            {
              std::string_view sep = "";
              nth::string_printer sp(false_args);
              for (SsaValue arg : b.false_arguments()) {
                f(sp, arg);
                sp.write(std::exchange(sep, ", "));
              }
            }

            nth::Interpolate<"  cond br {} ? #{} <- ({}) : #{} <- ({})\n">(
                p, f, b.value.reg(), b.true_block, true_args, b.false_block,
                false_args);
          } else if constexpr (t == nth::type<ReturnImpl>) {
            p.write("  return\n");
          }
        },
        branch.branch_);
  }

 private:
  struct UnreachableImpl {};
  struct UnconditionalImpl {
    size_t block;
    std::vector<SsaValue> block_arguments;
  };
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

  constexpr std::span<SsaValue const> parameters() const { return parameters_; }

  friend void NthPrint(auto &p, auto &f, SsaBasicBlock const &block) {
    p.write("block(");
    std::string_view separator = "";
    for (SsaValue v : block.parameters_) {
      p.write(std::exchange(separator, ", "));
      f(p, v.reg());
    }
    p.write("):\n");
    for (SsaInstruction const &i : block.instructions_) {
      f(p, i);
      p.write("\n");
    }
    f(p, block.branch_);
  }

  std::span<SsaInstruction const> instructions() const { return instructions_; }

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
    Initialize(DecodeOpCode<Set>, f.raw_instructions(),
               internal::BuiltinPointers<Set>);
  }

  std::span<SsaBasicBlock const> blocks() const { return blocks_; }

  friend void NthPrint(auto &p, auto &f, SsaFunction const &fn) {
    size_t i = 0;
    for (SsaBasicBlock const &block : fn.blocks_) {
      p.write("#");
      f(p, i++);
      p.write(" ");
      f(p, block);
      p.write("\n");
    }
  }

 private:
  void Initialize(OpCodeMetadata (*decode)(Value),
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
