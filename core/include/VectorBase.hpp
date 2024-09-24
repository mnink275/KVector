#pragma once

#include <type_traits>

namespace ink::impl {

template <class Alloc, bool kIsEmptyClass>
class MemoryManager;

template <class Alloc>
class MemoryManager<Alloc, true> : public Alloc {
 public:
  Alloc& GetAllocator() {
    return *this;
  }

  const Alloc& GetAllocator() const {
    return *this;
  }
};

template <class Alloc>
class MemoryManager<Alloc, false> {
 public:
  Alloc& GetAllocator() {
    return allocator_;
  }

  const Alloc& GetAllocator() const {
    return allocator_;
  }

 private:
  Alloc allocator_;
};

template <class T, class Alloc>
class VectorBase : public MemoryManager<Alloc, std::is_empty_v<Alloc>> {};

}  // namespace ink::impl
