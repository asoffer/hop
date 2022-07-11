#ifndef JASMIN_VALUE_STACK_H
#define JASMIN_VALUE_STACK_H

#include <tuple>
#include <deque>
#include <initializer_list>

#include "value.h"

namespace jasmin {

struct ValueStack {
  ValueStack(std::initializer_list<Value> list)
      : values_(list.begin(), list.end()) {}

  constexpr void push(Value const &v) { values_.push_back(v); }
  constexpr void push(SmallTrivialValue auto v) { values_.emplace_back(v); }

  Value pop_value() {
    assert(values_.size() > 0);
    Value result = values_.back();
    values_.pop_back();
    return result;
  }

  void swap_with(size_t n) {
    assert(values_.size() > n);
    auto iter       = values_.rbegin();
    Value &lhs      = *iter;
    Value &rhs      = *std::next(iter, n);
    Value temporary = lhs;
    lhs             = rhs;
    rhs             = temporary;
  }

  template <SmallTrivialValue T>
  T pop() {
    assert(values_.size() > 0);
    return pop_value().as<T>();
  }

  constexpr Value peek_value() const {
    assert(values_.size() != 0);
    return values_.back();
  }

  template <SmallTrivialValue... Ts>
  std::tuple<Ts...> pop_suffix() {
    assert(values_.size() >= sizeof...(Ts));
    auto iter = std::next(values_.rbegin(), sizeof...(Ts));
    auto result = std::tuple<Ts...>((--iter)->template as<Ts>()...);
    values_.resize(values_.size() - sizeof...(Ts));
    return result;
  }

  template <SmallTrivialValue T>
  constexpr T peek() const {
    return peek_value().as<T>();
  }

  void erase(size_t start, size_t end) {
    auto iter = values_.begin();
    values_.erase(iter + start, iter + end);
  }

  constexpr size_t size() const { return values_.size(); }

  // private:
  std::deque<Value> values_;
};

}  // namespace jasmin

#endif  // JASMIN_VALUE_STACK_H
