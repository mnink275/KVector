#pragma once

#include <memory>
#include <type_traits>
#include <vector>

namespace ink {

template <class T, class Allocator>
class KVector;

template <class T, bool IsConst>
class CommonIterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

  using iterator = CommonIterator<T, false>;
  using const_iterator = CommonIterator<T, true>;

 public:
  explicit CommonIterator(pointer ptr) noexcept : ptr_(ptr){};

  CommonIterator(const CommonIterator<T, false>& non_const_it) noexcept
      : ptr_(non_const_it.ptr_) {}

  CommonIterator& operator+=(const difference_type shift) noexcept {
    ptr_ += shift;
    return *this;
  }

  CommonIterator& operator-=(const difference_type shift) noexcept {
    ptr_ -= shift;
    return *this;
  }

  std::conditional_t<IsConst, const value_type&, value_type&> operator*()
      const noexcept {
    return *ptr_;
  }

  CommonIterator& operator++() noexcept {
    ++ptr_;
    return *this;
  }

  CommonIterator& operator--() noexcept {
    --ptr_;
    return *this;
  }

  std::conditional_t<IsConst, const pointer, pointer> operator->()
      const noexcept {
    return ptr_;
  }

  friend bool operator!=(const CommonIterator& left,
                         const CommonIterator& right) {
    return left.ptr_ != right.ptr_;
  }

  friend bool operator==(const CommonIterator& left,
                         const CommonIterator& right) {
    return left.ptr_ == right.ptr_;
  }

  friend difference_type operator-(const CommonIterator& lhs,
                                   const CommonIterator& rhs) noexcept {
    return (lhs.ptr_ - rhs.ptr_);
  }

 private:
  pointer ptr_;
};

template <class T, bool IsConst>
using diff_type = typename CommonIterator<T, IsConst>::difference_type;

template <class T, bool IsConst>
CommonIterator<T, IsConst> operator+(
    const CommonIterator<T, IsConst>& it,
    const diff_type<T, IsConst> shift) noexcept {
  auto tmp = it;
  tmp += shift;
  return tmp;
}

template <class T, bool IsConst>
CommonIterator<T, IsConst> operator+(
    const diff_type<T, IsConst> shift,
    const CommonIterator<T, IsConst>& it) noexcept {
  auto tmp = it;
  tmp += shift;
  return tmp;
}

template <class T, bool IsConst>
CommonIterator<T, IsConst> operator-(
    const CommonIterator<T, IsConst>& it,
    const diff_type<T, IsConst> shift) noexcept {
  auto tmp = it;
  tmp -= shift;
  return tmp;
}

}  // namespace ink