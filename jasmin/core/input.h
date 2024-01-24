#ifndef JASMIN_CORE_INPUT_H
#define JASMIN_CORE_INPUT_H

#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {

template <typename... Ts>
struct in {
  template <int N>
  auto get() {
    return (ptr_ + N * sizeof(Value))
        ->as<nth::type_t<types.template get<N>()>>();
  }

 private:
  static constexpr auto types = nth::type_sequence<Ts...>;
  Value *ptr_;
};

}  // namespace jasmin

#endif  // JASMIN_CORE_INPUT_H
