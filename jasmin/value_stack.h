#ifndef JASMIN_VALUE_STACK_H
#define JASMIN_VALUE_STACK_H

#include <initializer_list>
#include <iterator>
#include <tuple>
#include <vector>

#include "jasmin/internal/debug.h"
#include "jasmin/value.h"

namespace jasmin {

// A stack of `Value`s to be used as a core component in the stack machine.
struct ValueStack {
  // Constructs an empty `ValueStack`.
  ValueStack() = default;

  // Construts a value stack with the given initializer_list elements pushed
  // onto the stack in the same order as they appear in `list`.
  ValueStack(std::initializer_list<Value> list)
      : values_(list.begin(), list.end()) {}

  // Returns the number of elements on the stack.
  constexpr size_t size() const { return values_.size(); }

  // Returns true if the stack has no elements and false otherwise.
  constexpr bool empty() const { return values_.empty(); }

  // Pushes the given value `v` onto the stack.
  constexpr void push(Value const &v) { values_.push_back(v); }
  constexpr void push(SmallTrivialValue auto v) { values_.emplace_back(v); }

  // Pop the top `Value` off the stack and return it. Behavior is undefined if
  // the stack is empty.
  Value pop_value() {
    JASMIN_INTERNAL_DEBUG_ASSERT(not values_.empty(),
                                 "Unexpectedly empty ValueStack");
    Value result = values_.back();
    values_.pop_back();
    return result;
  }

  // Pop a value of type `T` off the stack and return it. Behavior is undefined
  // if the stack is empty or the top value is not of type `T`.
  template <SmallTrivialValue T>
  T pop() {
    return pop_value().as<T>();
  }

  // Returns a copy of the top `Value` on the stack. Behavior is undefined if
  // the stack is empty.
  constexpr Value peek_value() const {
    JASMIN_INTERNAL_DEBUG_ASSERT(not values_.empty(),
                                 "Unexpectedly empty ValueStack");
    return values_.back();
  }

  // Returns a copy of the top value of type `T` on the stack. Behavior is
  // undefined if the stack is empty or the top value is not of type `T`.
  template <SmallTrivialValue T>
  constexpr T peek() const {
    return peek_value().as<T>();
  }

  // Swaps the top of the stack with the element `n` entries from the top of the
  // stack. Behavior is undefined if `n` is zero, or if `n` is larger than the
  // number of elements in the stack.
  void swap_with(size_t n) {
    JASMIN_INTERNAL_DEBUG_ASSERT(
        n != 0, "Unexpectedly attempting to swap an element with itself");
    JASMIN_INTERNAL_DEBUG_ASSERT(values_.size() > n,
                                 "Unexpectedly too few elements in ValueStack");
    auto iter       = values_.rbegin();
    Value &lhs      = *iter;
    Value &rhs      = *std::next(iter, n);
    Value temporary = lhs;
    lhs             = rhs;
    rhs             = temporary;
  }

  // Pops the last `sizeof...(Ts)` elements off the stack and returns a
  // `std:tuple<Ts...>` consisting of the values popped from the stack such that
  // the elements in the tuple are in the same order as they were in the stack.
  // In other words, the last entry in the tuple was at the top of the stack.
  // Behavior is undefined if the stack has fewer that `sizeof...(Ts)` elements
  // or if any of the stored elements do not match their assocaited type in
  // `Ts...`.
  template <SmallTrivialValue... Ts>
  std::tuple<Ts...> pop_suffix() {
    JASMIN_INTERNAL_DEBUG_ASSERT(values_.size() >= sizeof...(Ts),
                                 "Unexpectedly too few elements in ValueStack");
    auto iter   = std::next(values_.rbegin(), sizeof...(Ts));
    auto result = std::tuple<Ts...>((--iter)->template as<Ts>()...);
    ((static_cast<void>(static_cast<Ts const *>(nullptr)), values_.pop_back()),
     ...);
    return result;
  }

  // Erase elements from the stack starting at position `start` up to but not
  // including position `end`. If `start == end` no elements are removed. Note
  // that positions are measured from the bottom, rather than the top, of the
  // stack. Behavior is undefined if `start > end`, or if `end` is greater than
  // the size of the stack.
  void erase(size_t start, size_t end) {
    JASMIN_INTERNAL_DEBUG_ASSERT(start <= end,
                                 "Unexpectedly invalid range to erase");
    JASMIN_INTERNAL_DEBUG_ASSERT(end <= values_.size(),
                                 "Unexpectedly too few elements in ValueStack");

    auto iter = values_.begin();
    values_.erase(iter + start, iter + end);
  }

 private:
  std::vector<Value> values_;
};

}  // namespace jasmin

#endif  // JASMIN_VALUE_STACK_H
