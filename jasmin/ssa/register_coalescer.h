#ifndef JASMIN_SSA_REGISTER_COALESCER_H
#define JASMIN_SSA_REGISTER_COALESCER_H

#include "jasmin/core/instruction.h"
#include "jasmin/ssa/ssa.h"
#include "nth/container/disjoint_set.h"

namespace jasmin {

struct RegisterCoalescer {
  void identify(SsaValue a, SsaValue b);

  void rename(SsaBranch& b);
  void rename(SsaValue& v);
  void rename(SsaInstruction& i);

  template <InstructionSetType Set>
  void Coalesce(SsaFunction& f);

 private:
  using handle_type = nth::disjoint_set<SsaValue>::handle;

  void freshen();
  SsaValue get(handle_type h);

  absl::flat_hash_map<handle_type, handle_type> constants_;
  nth::disjoint_set<SsaValue> set_;
  bool stale_ = false;
};

namespace internal {

template <InstructionSetType Set>
absl::flat_hash_map<internal::exec_fn_type,
                    void (*)(RegisterCoalescer&, SsaInstruction const&)>
    coalescing_functions = [] {
      absl::flat_hash_map<internal::exec_fn_type,
                          void (*)(RegisterCoalescer&, SsaInstruction const&)>
          op_code_map;

      Set::instructions.each([&](auto inst_type) {
        using type = nth::type_t<inst_type>;
        if constexpr (requires(RegisterCoalescer & rc,
                               SsaInstruction const& i) {
                        { type::identify(rc, i) } -> std::same_as<void>;
                      }) {
          op_code_map.emplace(&type::template ExecuteImpl<Set>, type::identify);
        }
      });

      return op_code_map;
    }();

}  // namespace internal

template <InstructionSetType Set>
void RegisterCoalescer::Coalesce(SsaFunction& f) {
  for (auto& block : f.blocks()) {
    for (auto& i : block.instructions()) {
      if (auto iter = internal::coalescing_functions<Set>.find(i.op_code());
          iter != internal::coalescing_functions<Set>.end()) {
        iter->second(*this, i);
      }
    }
  }
  for (auto& block : f.blocks()) {
    for (auto& param : block.parameters()) { rename(param); }

    auto read_iter  = block.instructions().begin();
    auto write_iter = block.instructions().begin();
    while (read_iter != block.instructions().end()) {
      if (internal::coalescing_functions<Set>.contains(read_iter->op_code())) {
        ++read_iter;
      } else {
        auto& w = *write_iter++;
        w       = *read_iter++;
        rename(w);
      }
    }
    size_t n = std::distance(write_iter, block.instructions().end());
    for (size_t i = 0; i < n; ++i) { block.remove_back(); }
    SsaBranch branch = block.branch();
    rename(branch);
    block.set_branch(std::move(branch));
  }
}

}  // namespace jasmin

#endif  // JASMIN_SSA_REGISTER_COALESCER_H
