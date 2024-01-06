#include "jasmin/ssa/register_coalescer.h"

#include "nth/debug/debug.h"

namespace jasmin {

void RegisterCoalescer::identify(SsaValue a, SsaValue b) {
  auto ha = set_.insert(a).first;
  if (not a.is_register()) { constants_.emplace(ha, ha); }
  auto hb = set_.insert(b).first;
  if (not b.is_register()) { constants_.emplace(hb, hb); }
  set_.join(ha, hb);
  stale_ = true;
}

void RegisterCoalescer::rename(SsaBranch& b) {
  freshen();
  b.rename(set_);
}

void RegisterCoalescer::rename(SsaValue& v) {
  if (auto h = set_.find_representative(v); not h.empty()) {
    freshen();
    v = get(h);
  }
}

void RegisterCoalescer::rename(SsaInstruction& i) {
  freshen();
  for (auto& v : i.arguments()) {
    if (auto h = set_.find_representative(v); not h.empty()) { v = get(h); }
  }
  for (auto& v : i.outputs()) {
    if (auto h = set_.find_representative(v); not h.empty()) { v = get(h); }
  }
}

void RegisterCoalescer::freshen() {
  if (stale_) {
    auto c = std::exchange(constants_, {});
    for (auto [h, constant] : c) {
      constants_.emplace(set_.representative(h), constant);
    }
  }
  stale_ = true;
}

SsaValue RegisterCoalescer::get(handle_type h) {
  NTH_REQUIRE((v.debug), not h.empty());
  NTH_REQUIRE((v.debug), not stale_);
  if (auto iter = constants_.find(h); iter != constants_.end()) {
    return *iter->second;
  } else {
    return *h;
  }
}

}  // namespace jasmin
