#pragma once

#include <iostream>
#include <iterator>
#include <vector>

#include "Iterator.hpp"

namespace ink {

template <class T, class Allocator = std::allocator<T>>
class KVector final {
 private:
  using alloc_traits = std::allocator_traits<Allocator>;

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
    for (int i = 0; i < size_; ++i) {
      alloc_traits::destroy(alloc_, buffer_ + i);
    }
    // clearing memory
    alloc_traits::deallocate(alloc_, buffer_, capacity_);
  }

  KVector(std::size_t size, const value_type& value,
          const Allocator& alloc = Allocator()) {}

  template <class InputIt>
  KVector(InputIt first, InputIt last) {
    auto new_capacity = std::distance(first, last);
    reserve(new_capacity);
    while (first != last) {
      emplace_back(*first);
      ++first;
    }
  }

  // Element access
  value_type& operator[](std::size_t idx) const noexcept {
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

  std::size_t size() const noexcept { return size_; }

  void reserve(std::size_t new_capacity) {
    if (new_capacity > capacity_) {
      reallocateBuffer(new_capacity);
    }
  }

  std::size_t capacity() const noexcept { return capacity_; }

  void shrink_to_fit() {
    if (capacity_ > size_) {
      reallocateBuffer(size_);
    }
  }

  // Modifiers
  void clear() noexcept {
    for (int i = 0; i < size_; ++i) {
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

  void resize(std::size_t new_size) { resizeVector(new_size); }

  void resize(std::size_t new_size, const value_type& value) {
    resizeVector(new_size, value);
  }

 private:
  void reallocateBuffer(std::size_t new_capacity) {
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
    int idx = 0;
    try {
      // constructing new data using copy-constructor
      for (; idx < size_; ++idx) {
        alloc_traits::construct(alloc_, new_buffer + idx, old_buffer[idx]);
      }
    } catch (...) {
      // calling the new data destructors
      for (int del_idx = 0; del_idx < idx; ++del_idx) {
        alloc_traits::destroy(alloc_, new_buffer + del_idx);
      }
      throw;
    }
    // calling the old data destructors
    for (int i = 0; i < size_; ++i) {
      alloc_traits::destroy(alloc_, old_buffer + i);
    }
  }

  void createBufferMove(pointer new_buffer, pointer old_buffer) {
    int idx = 0;
    // constructing new data using move-constructor
    for (; idx < size_; ++idx) {
      alloc_traits::construct(alloc_, new_buffer + idx,
                              std::move(old_buffer[idx]));
    }
    // calling the old data destructors
    for (int i = 0; i < size_; ++i) {
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
        for (int del_idx = size_; del_idx < idx; ++del_idx) {
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

}  // namespace ink