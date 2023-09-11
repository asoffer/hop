#ifndef JASMIN_VALUE_STACK_H
#define JASMIN_VALUE_STACK_H

#include <initializer_list>
#include <memory>
#include <tuple>

#include "jasmin/internal/debug.h"
#include "jasmin/internal/type_traits.h"
#include "jasmin/value.h"

namespace jasmin {

// A stack of `Value`s to be used as a core component in the stack machine.
struct ValueStack {
  // Constructs an empty `ValueStack`.
  ValueStack()
      : cap_(16),
        values_(static_cast<Value *>(operator new[](16 * sizeof(Value)))),
        head_(values_.get()) {
    for (size_t i = 0; i < cap_; ++i) {
      new (&values_[i]) Value(Value::Uninitialized());
    }
  }

  // Construts a value stack with the given initializer_list elements pushed
  // onto the stack in the same order as they appear in `list`.
  ValueStack(std::initializer_list<Value> list) : ValueStack() {
    for (Value v : list) { push(v); }
  }

  // Returns the number of elements on the stack.
  constexpr size_t size() const { return head_ - values_.get(); }

  // Returns true if the stack has no elements and false otherwise.
  constexpr bool empty() const { return size() == 0; }

  // Pushes the given value `v` onto the stack.
  void push(Value const &v) {
    if (head_ == values_.get() + cap_) [[unlikely]] {
        reallocate();
      }
    JASMIN_INTERNAL_DEBUG_ASSERT(head_ != values_.get() + cap_,
                                 "Something went wrong with reallocate()");
    *head_ = v;
    ++head_;
  }
  void push(SmallTrivialValue auto v) { push(Value(v)); }

  using const_iterator  = Value const *;

  // Returns an iterator referrencing one passed the end. of the `ValueStack`.
  const_iterator begin() const { return values_.get(); }
  const_iterator end() const { return head_; }

  // Pop the top `Value` off the stack and return it. Behavior is undefined if
  // the stack is empty.
  Value pop_value() {
    JASMIN_INTERNAL_DEBUG_ASSERT(not empty(), "Unexpectedly empty ValueStack");
    return *--head_;
  }

  // Pop a value of type `T` off the stack and return it. Behavior is undefined
  // if the stack is empty or the top value is not of type `T`.
  template <SmallTrivialValue T>
  T pop() {
    return pop_value().as<T>();
  }

  // Returns a copy of the `Value` that is `count_back` entries from the top of
  // the stack. Behavior is undefined if the stack does not contain at least
  // `count_back + 1` values.
  constexpr Value peek_value(size_t count_back = 0) const {
    JASMIN_INTERNAL_DEBUG_ASSERT(size() > count_back,
                                 "Unexpectedly short ValueStack");
    return *(head_ - (count_back + 1));
  }

  // Returns a copy of the value of type `T` that is `count_back` entries from
  // the top of the stack. Behavior is undefined if the stack does not contain
  // at least `count_back + 1` values, or the value is not of type `T`.
  template <SmallTrivialValue T>
  constexpr T peek(size_t count_back = 0) const {
    return peek_value(count_back).as<T>();
  }

  // Swaps the top of the stack with the element `n` entries from the top of the
  // stack. Behavior is undefined if `n` is zero, or if `n` is larger than the
  // number of elements in the stack.
  void swap_with(size_t n) {
    JASMIN_INTERNAL_DEBUG_ASSERT(
        n != 0, "Unexpectedly attempting to swap an element with itself");
    JASMIN_INTERNAL_DEBUG_ASSERT(size() > n,
                                 "Unexpectedly too few elements in ValueStack");
    auto *p = head_ - 1;
    std::swap(*p, *(p - n));
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
    JASMIN_INTERNAL_DEBUG_ASSERT(size() >= sizeof...(Ts),
                                 "Unexpectedly too few elements in ValueStack");
    head_ -= sizeof...(Ts);
    auto *p = head_;
    return std::tuple<Ts...>{(p++)->template as<Ts>()...};
  }

  // TODO: qualify `F`.
  template <auto F, typename... Ts, typename... Args>
  void call_on_suffix(Args &&...args) {
    if constexpr (sizeof...(Ts) == 0) {
      push(F(std::forward<Args>(args)...));
    } else if constexpr (sizeof...(Ts) == 1) {
      auto *p = head_ - 1;
      *p      = F(std::forward<Args>(args)..., p->as<Ts...>());
    } else {
      auto result = std::apply(
          [&](auto... suffix) {
            return F(std::forward<Args>(args)..., suffix...);
          },
          pop_suffix<Ts...>());
      *head_      = result;
      ++head_;
    }
  }

  // Erase elements from the stack starting at position `start` up to but not
  // including position `end`. If `start == end` no elements are removed. Note
  // that positions are measured from the bottom, rather than the top, of the
  // stack. Behavior is undefined if `start > end`, or if `end` is greater than
  // the size of the stack.
  void erase(size_t start, size_t end) {
    JASMIN_INTERNAL_DEBUG_ASSERT(start <= end,
                                 "Unexpectedly invalid range to erase");
    JASMIN_INTERNAL_DEBUG_ASSERT(end <= size(),
                                 "Unexpectedly too few elements in ValueStack");

    std::memcpy(values_.get() + start, values_.get() + end,
                sizeof(Value) * (head_ - values_.get() - end));
    head_ -= end - start;
  }

 private:
  void reallocate() {
    std::unique_ptr<Value[]> buffer(
        static_cast<Value *>(operator new[](2 * cap_ * sizeof(Value))));

    std::memcpy(buffer.get(), values_.get(), sizeof(Value) * cap_);

    // This whole loop we really hope to be optimized down to a no-op, but it is
    // technically necessary to start the lifetime of `Value`s in the back half
    // of the buffer.
    for (size_t i = cap_; i < cap_ * 2; ++i) {
      buffer[i] = Value::Uninitialized();
    }

    head_ = buffer.get() + cap_;
    cap_ *= 2;
    values_ = std::move(buffer);
  }

  size_t cap_;
  std::unique_ptr<Value[]> values_;
  Value *head_;
};

}  // namespace jasmin

#endif  // JASMIN_VALUE_STACK_H
