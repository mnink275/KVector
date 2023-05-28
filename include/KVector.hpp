#pragma once

#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace ink {

template <class T, class Alloc = std::allocator<T>>
class KVector final {
 public:
  using alloc_traits = std::allocator_traits<Alloc>;

  using value_type = T;
  using pointer = T*;

 public:
  KVector() = default;
  ~KVector() {
    for (int i = 0; i < size_; ++i) {
      alloc_traits::destroy(alloc_, buffer_ + i);
    }
    alloc_traits::deallocate(alloc_, buffer_, capacity_);
  }

  KVector(std::size_t size, const value_type& value,
          const Alloc& alloc = Alloc()) {}

  value_type& operator[](std::size_t idx) const noexcept {
    return *(buffer_ + idx);
  }

  // Capacity
  bool empty() const noexcept {
    return size_ == 0;
  }

  std::size_t size() const  noexcept {
    return size_;
  }

  void reserve(std::size_t new_capacity) {
    if (new_capacity > capacity_) {
      reallocateBuffer(2 * new_capacity + 1);
    }
  }

  std::size_t capacity() const noexcept {
    return capacity_;
  }

  void shrink_to_fit() {
    if (capacity_ > size_) {
      alloc_traits::deallocate(alloc_, buffer_ + size_, capacity_ - size_);
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
    if (size_ == capacity_) {
      reserve(2 * capacity_ + 1);
    }

    const auto& construction_place = size_ == 0 ? buffer_ : buffer_ + size_;
    alloc_traits::construct(alloc_, construction_place,
                            std::forward<SomeType>(value));
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
        createBufferCopy(new_buffer, buffer_);
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
      for (; idx < size_; ++idx) {
        alloc_traits::construct(alloc_, new_buffer + idx, old_buffer[idx]);
      }
    } catch (...) {
      for (int del_idx = 0; del_idx < idx; ++del_idx) {
        alloc_traits::destroy(alloc_, new_buffer + del_idx);
      }
      throw;
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
  std::allocator<value_type> alloc_; // TODO: EBCO

  pointer buffer_{nullptr};
};

}  // namespace ink