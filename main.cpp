#include <iostream>
#include <type_traits>
#include <vector>
#include <exception>
#include <numeric>

#include "include/KVector.hpp"

struct NonDefaultConstr {
  NonDefaultConstr() = delete;
  NonDefaultConstr(int new_num) : num(new_num) {}
  NonDefaultConstr(const NonDefaultConstr&) = delete;
  NonDefaultConstr& operator=(const NonDefaultConstr&) = delete;
  NonDefaultConstr(NonDefaultConstr&&) = delete;
  NonDefaultConstr& operator=(NonDefaultConstr&&) = delete;
  int num;
};

struct Kek {
  long long* ptr{nullptr};

  Kek() {
    std::cout << "Kek()\n";
    ptr = new long long(5);
  }

  ~Kek() {
    std::cout << "~Kek()\n";
    delete ptr;
  }
  Kek(const Kek& some_kek) {
    std::cout << "Kek(const Kek&)\n";
    ptr = new long long(*(some_kek.ptr));
  }
  Kek(Kek&& some_kek) noexcept {
    std::cout << "Kek(Kek&&)\n";
    delete ptr;
    ptr = some_kek.ptr;
    some_kek.ptr = nullptr;
  }
};

int main() {
  ink::KVector<int> vec;
  for (int i = 0; i < 5; ++i) {
    vec.push_back(i);
  }

  auto it = vec.begin();
  std::cout << *it;
  std::cout << "\n";

  for (auto&& num : vec) {
    std::cout << num << " ";
  }
  std::cout << "\n";
  // vec.reserve(5);
  // vec.push_back(1);
  // vec.push_back(2);
  // vec.shrink_to_fit();

  // Kek kek;
  // ink::KVector<Kek> kek_vec;
  // try {
  //   kek_vec.push_back(kek);
  // } catch (std::exception& e) {
  //   std::cout << e.what() << "\n";
  // }

  // ink::KVector<Kek> kek_vec;
  // for (int i = 0; i < 2; ++i) {
  //   Kek kek;
  //   kek_vec.push_back(std::move(kek));
  // }

  //kek_vec.push_back(std::move(kek));

  // ink::KVector<int> vec;
  // for (int i = 0; i < 5; ++i) {
  //   vec.push_back(i);
  //   std::cout << vec.capacity() << " ";
  // }
  // std::cout << "\n";
}