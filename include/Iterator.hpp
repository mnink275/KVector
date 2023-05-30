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

 private:
  pointer ptr_;
};

}  // namespace ink