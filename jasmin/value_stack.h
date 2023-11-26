#ifndef JASMIN_VALUE_STACK_H
#define JASMIN_VALUE_STACK_H

#include <initializer_list>
#include <memory>
#include <tuple>

#include "jasmin/internal/call_stack.h"
#include "jasmin/value.h"
#include "nth/debug/debug.h"

namespace jasmin {
struct ValueStack;

namespace internal {

void ReallocateValueStack(Value *value_stack_head, size_t, Value const *ip,
                          FrameBase *call_stack, uint64_t cap_and_left) {
  size_t capacity = value_stack_head->as<size_t>();
  size_t bytes    = capacity * sizeof(Value);

  Value *new_ptr = static_cast<Value *>(operator new(2 * bytes));
  std::memcpy(new_ptr, value_stack_head - (capacity - 1), bytes);
  *(new_ptr + (2 * capacity - 1)) = static_cast<size_t>(2 * capacity);

  operator delete(value_stack_head - (capacity - 1));

  NTH_ATTRIBUTE(tailcall)
  return ip->template as<exec_fn_type>()(new_ptr + capacity - 1, capacity, ip,
                                         call_stack, cap_and_left);
}

ValueStack Concoct(Value *, uint64_t remaining);
void Ignore(ValueStack &);

}  // namespace internal

struct ValueStackRef {
  constexpr ValueStackRef(ValueStackRef const &)            = default;
  constexpr ValueStackRef &operator=(ValueStackRef const &) = default;

  constexpr ValueStackRef(ValueStackRef &&v)
      : head_(std::exchange(v.head_, nullptr)),
        left_(std::exchange(v.left_, 0)) {}

  constexpr ValueStackRef &operator=(ValueStackRef &&v) {
    head_ = std::exchange(v.head_, nullptr);
    left_ = std::exchange(v.left_, 0);
    return *this;
  }

  // Returns the number of elements on the stack.
  constexpr size_t size() const { return capacity() - space_remaining() - 1; }

  constexpr size_t space_remaining() const { return left_; }

  constexpr size_t capacity() const {
    return head_ ? (head_ + space_remaining())->as<size_t>() : 1;
  }

  constexpr Value *head() { return head_; }
  constexpr Value const *head() const { return head_; }

  // Returns true if the stack has no elements and false otherwise.
  constexpr bool empty() const { return size() == 0; }

  // Pushes the given value `v` onto the stack.
  constexpr void push(Value const &v) {
    if (space_remaining() == 0) [[unlikely]] { reallocate(); }
    *head_++ = v;
    --left_;
  }
  constexpr void push(SmallTrivialValue auto v) { push(Value(v)); }

  using const_iterator = Value const *;
  using iterator       = Value *;

  // Returns an iterator referrencing the first entry of the `ValueStack`.
  constexpr iterator begin() { return head_ - size(); }
  constexpr const_iterator begin() const { return head_ - size(); }
  // Returns an iterator referrencing one passed the end of the `ValueStack`.
  constexpr iterator end() { return head_; }
  constexpr const_iterator end() const { return head_; }

  // Pop the top `Value` off the stack and return it. Behavior is undefined if
  // the stack is empty.
  Value pop_value() {
    NTH_REQUIRE((v.when(internal::harden)), not empty())
        .Log<"Unexpectedly empty ValueStack">();
    ++left_;
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
  Value peek_value(size_t count_back = 0) const {
    NTH_REQUIRE((v.when(internal::harden)), size() > count_back)
        .Log<"Unexpectedly short ValueStack">();
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
    NTH_REQUIRE((v.when(internal::harden)), n != size_t{0})
        .Log<"Unexpectedly attempting to swap an element with itself">();
    NTH_REQUIRE((v.when(internal::harden)), size() > n)
        .Log<"Unexpectedly too few elements in ValueStack">();
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
    NTH_REQUIRE((v.when(internal::harden)), size() >= sizeof...(Ts))
        .Log<"Unexpectedly too few elements in ValueStack">();
    head_ -= sizeof...(Ts);
    left_ += sizeof...(Ts);
    auto *p = head_;
    return std::tuple<Ts...>{(p++)->template as<Ts>()...};
  }

  template <auto F, typename... Ts, typename... Args>
  requires std::invocable<decltype(F), Args..., Ts...>
  void call_on_suffix(Args &&...args) {
    if constexpr (std::is_void_v<
                      std::invoke_result_t<decltype(F), Args..., Ts...>>) {
      if constexpr (sizeof...(Ts) == 0) {
        F(std::forward<Args>(args)...);
      } else {
        auto *p = head_ - sizeof...(Ts);
        [&]<size_t... Ns>(std::index_sequence<Ns...>) {
          F(std::forward<Args>(args)..., (p + Ns)->template as<Ts>()...);
        }
        (std::make_index_sequence<sizeof...(Ts)>{});
        head_ -= sizeof...(Ts);
        left_ += sizeof...(Ts);
      }
    } else {
      if constexpr (sizeof...(Ts) == 0) {
        push(F(std::forward<Args>(args)...));
      } else {
        auto *p = head_ - sizeof...(Ts);
        [&]<size_t... Ns>(std::index_sequence<Ns...>) {
          *p = F(std::forward<Args>(args)..., (p + Ns)->template as<Ts>()...);
        }
        (std::make_index_sequence<sizeof...(Ts)>{});
        head_ -= (sizeof...(Ts) - 1);
        left_ += (sizeof...(Ts) - 1);
      }
    }
  }

  // Erase elements from the stack starting at position `start` up to but not
  // including position `end`. If `start == end` no elements are removed. Note
  // that positions are measured from the bottom, rather than the top, of the
  // stack. Behavior is undefined if `start > end`, or if `end` is greater than
  // the size of the stack.
  void erase(size_t start, size_t end) {
    NTH_REQUIRE((v.when(internal::harden)), start <= end)
        .Log<"Unexpectedly invalid range to erase">();
    NTH_REQUIRE((v.when(internal::harden)), end <= size())
        .Log<"Unexpectedly too few elements in ValueStack">();

    std::memmove(head_ - (size() - start), head_ - (size() - end),
                 sizeof(Value) * (size() - end));
    head_ -= end - start;
    left_ += end - start;
  }

  static constexpr ValueStackRef Push(ValueStackRef vsr, Value v) {
    vsr.push(v);
    return vsr;
  }

  static constexpr ValueStackRef EraseLast(ValueStackRef vsr, size_t count) {
    NTH_REQUIRE((v.when(internal::harden)), count <= vsr.size())
        .Log<"Unexpectedly invalid range to erase">();
    return ValueStackRef(vsr.head_ - count, vsr.left_ + count);
  }

  constexpr ValueStackRef(Value *head, size_t remaining)
      : head_(head), left_(remaining) {}

 protected:
  Value *head_;
  uint64_t left_;

 private:
  void reallocate() {
    size_t capacity = head_->as<size_t>();
    size_t bytes    = capacity * sizeof(Value);

    Value *new_ptr = static_cast<Value *>(operator new(2 * bytes));
    auto *q        = head_ - (capacity - 1);
    for (auto *p = new_ptr; p != new_ptr + bytes; ++p, ++q) { *p = *q; }
    *(new_ptr + (2 * capacity - 1)) = static_cast<size_t>(2 * capacity);

    operator delete(head_ - (capacity - 1));
    head_ = new_ptr + capacity - 1;
    left_ = capacity;
  }
};

// A stack of `Value`s to be used as a core component in the stack machine.
struct ValueStack : ValueStackRef {
  // Constructs an empty `ValueStack`.
  ValueStack()
      : ValueStackRef(static_cast<Value *>(operator new(16 * sizeof(Value))),
                      15) {
    *(head() + 15) = 16;
  }

  ValueStack(ValueStack &&)            = default;
  ValueStack &operator=(ValueStack &&) = default;

  // Construts a value stack with the given initializer_list elements pushed
  // onto the stack in the same order as they appear in `list`.
  ValueStack(std::initializer_list<Value> list) : ValueStack() {
    for (Value v : list) { push(v); }
  }

  ~ValueStack() { operator delete(head() - size()); }

 private:
  friend ValueStack internal::Concoct(Value *, uint64_t remaining);
  friend void internal::Ignore(ValueStack &);

  ValueStack(Value *head, size_t remaining) : ValueStackRef(head, remaining) {}

  void ignore() {
    head_ = nullptr;
    left_ = 0;
  }
};

namespace internal {
inline ValueStack Concoct(Value *head, uint64_t remaining) {
  return ValueStack(head, remaining);
}

inline void Ignore(ValueStack &vs) { vs.ignore(); }

}  // namespace internal

}  // namespace jasmin

#endif  // JASMIN_VALUE_STACK_H
