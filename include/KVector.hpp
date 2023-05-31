#pragma once

#include <iostream>
#include <iterator>
#include <type_traits>

#include "Iterator.hpp"

namespace ink {

template <class T, class Allocator = std::allocator<T>>
class KVector final {
 private:
  using alloc_traits = std::allocator_traits<Allocator>;
  template <class InputIt>
  using iterator_category_t =
      typename std::iterator_traits<InputIt>::iterator_category;

 public:
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename alloc_traits::pointer;
  using const_pointer = typename alloc_traits::const_pointer;

  using iterator = CommonIterator<T, false>;
  using const_iterator = CommonIterator<T, false>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 public:
  KVector() = default;
  ~KVector() {
    // calling the data destructors
    for (size_type i = 0; i < size_; ++i) {
      alloc_traits::destroy(alloc_, buffer_ + i);
    }
    // clearing memory
    alloc_traits::deallocate(alloc_, buffer_, capacity_);
  }

  KVector(size_type size) { resize(size); }

  KVector(size_type size, const value_type& value) { resize(size, value); }

  // SFINAE overload (imagine we don't have C++20 )
  template <class InputIt,
            class ExtraT = std::enable_if_t<std::is_base_of_v<
                std::input_iterator_tag, iterator_category_t<InputIt>>>>
  KVector(InputIt first, InputIt last) {
    auto new_capacity = std::distance(first, last);
    reserve(new_capacity);
    while (first != last) {
      emplace_back(*first);
      ++first;
    }
  }

  // Element access
  value_type& operator[](size_type idx) const noexcept {
    return *(buffer_ + idx);
  }

  // Iterators
  iterator begin() { return iterator(buffer_); }
  const_iterator cbegin() { return const_iterator(buffer_); }

  iterator end() { return iterator(buffer_ + size_); }
  const_iterator cend() { return const_iterator(buffer_ + size_); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator crbegin() { return const_reverse_iterator(cend()); }

  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator crend() { return const_reverse_iterator(cbegin()); }

  // Capacity
  bool empty() const noexcept { return size_ == 0; }

  size_type size() const noexcept { return size_; }

  size_type max_size() const noexcept { return 1e9 / sizeof(value_type); }

  void reserve(size_type new_capacity) {
    if (new_capacity > capacity_) {
      reallocateBuffer(new_capacity);
    }
  }

  size_type capacity() const noexcept { return capacity_; }

  void shrink_to_fit() {
    if (capacity_ > size_) {
      reallocateBuffer(size_);
    }
  }

  // Modifiers
  void clear() noexcept {
    for (size_type i = 0; i < size_; ++i) {
      alloc_traits::destroy(alloc_, buffer_ + i);
    }
  }

  template <class SomeType>
  void push_back(SomeType&& value) {
    emplace_back(std::forward<SomeType>(value));
  }

  template <class... Args>
  void emplace_back(Args&&... args) {
    if (size_ == capacity_) {
      auto new_capacity = size_ == 0 ? 1 : 2 * capacity_;
      reserve(new_capacity);
    }

    const auto& construction_place = buffer_ + size_;
    alloc_traits::construct(alloc_, construction_place,
                            std::forward<Args>(args)...);
    ++size_;
  }

  void pop_back() noexcept {
    --size_;
    alloc_traits::destroy(alloc_, buffer_ + size_);
  }

  void resize(size_type new_size) { resizeVector(new_size); }

  void resize(size_type new_size, const value_type& value) {
    resizeVector(new_size, value);
  }

 private:
  void reallocateBuffer(size_type new_capacity) {
    pointer new_buffer = alloc_traits::allocate(alloc_, new_capacity);

    if (buffer_ != nullptr) {
      try {
        if constexpr (std::is_nothrow_move_constructible_v<value_type> ||
                      !std::is_copy_constructible_v<value_type>) {
          createBufferMove(new_buffer, buffer_);
        } else {
          createBufferCopy(new_buffer, buffer_);
        }
      } catch (...) {
        alloc_traits::deallocate(alloc_, new_buffer, new_capacity);
        throw;
      }
    }

    alloc_traits::deallocate(alloc_, buffer_, capacity_);
    buffer_ = new_buffer;
    capacity_ = new_capacity;
  }

  void createBufferCopy(pointer new_buffer, pointer old_buffer) {
    size_type idx = 0;
    try {
      // constructing new data using copy-constructor
      for (; idx < size_; ++idx) {
        alloc_traits::construct(alloc_, new_buffer + idx, old_buffer[idx]);
      }
    } catch (...) {
      // calling the new data destructors
      for (size_type del_idx = 0; del_idx < idx; ++del_idx) {
        alloc_traits::destroy(alloc_, new_buffer + del_idx);
      }
      throw;
    }
    // calling the old data destructors
    for (size_type i = 0; i < size_; ++i) {
      alloc_traits::destroy(alloc_, old_buffer + i);
    }
  }

  void createBufferMove(pointer new_buffer, pointer old_buffer) {
    size_type idx = 0;
    // constructing new data using move-constructor
    for (; idx < size_; ++idx) {
      alloc_traits::construct(alloc_, new_buffer + idx,
                              std::move(old_buffer[idx]));
    }
    // calling the old data destructors
    for (size_type i = 0; i < size_; ++i) {
      alloc_traits::destroy(alloc_, old_buffer + i);
    }
  }

  template <class... Args>
  void resizeVector(std::size_t new_size, Args&&... args) {
    if (new_size < size_) {
      while (new_size < size_) {
        pop_back();
      }
      return;
    }

    if (new_size > size_) {
      if (new_size > capacity_) {
        reallocateBuffer(new_size);
      }
      std::size_t idx = size_;
      try {
        for (; idx < new_size; ++idx) {
          alloc_traits::construct(alloc_, buffer_ + idx,
                                  std::forward<Args>(args)...);
        }
      } catch (...) {
        for (size_type del_idx = size_; del_idx < idx; ++del_idx) {
          alloc_traits::destroy(alloc_, buffer_ + del_idx);
        }
        throw;
      }

      size_ = new_size;
    }
  }

 private:
  std::size_t size_{0};
  std::size_t capacity_{0};
  std::allocator<value_type> alloc_;  // TODO: EBCO

  pointer buffer_{nullptr};
};

// deduction hint for CTAD
template <class InputIt>
KVector(InputIt first, InputIt last)
    -> KVector<typename std::iterator_traits<InputIt>::value_type>;

}  // namespace ink