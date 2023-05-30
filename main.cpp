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
  vec.reserve(5);
  for (int i = 0; i < 5; ++i) {
    vec.emplace_back(i);
  }

  std::vector<int> std_vec(5);
  auto std_it = std_vec.begin();
  int shift_int = 5;
  std_it += shift_int;
  
  long long shift_long = 5;
  std_it += shift_long;

  auto const_std_it = std_vec.cbegin();
  (const_std_it + 5);

  ink::KVector<int> new_vec(vec.begin(), vec.end());
  for (auto&& num : new_vec) {
    std::cout << num << " ";
  }
  std::cout << "\n";


  // auto it = vec.begin();
  // std::cout << *it;
  // std::cout << "\n";

  // for (auto&& num : vec) {
  //   std::cout << num << " ";
  // }
  // std::cout << "\n";
}