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

  ink::KVector<int> iter_vec(vec.begin(), vec.end());
  for (auto&& num : iter_vec) {
    std::cout << num << " ";
  }
  std::cout << "\n";

  // max_size
  ink::KVector<char> char_vec;
  std::cout << char_vec.max_size() << "\n";

  // std::vector<NonDefaultConstr> std_vec;
  // std_vec.emplace_back(5);
  // std_vec.emplace_back(5);
  // std_vec.emplace_back(5);
  ink::KVector<NonDefaultConstr> std_vec;
  std_vec.emplace_back(5);
  std_vec.emplace_back(5);
  std_vec.emplace_back(5);


  // auto it = vec.begin();
  // std::cout << *it;
  // std::cout << "\n";

  // for (auto&& num : vec) {
  //   std::cout << num << " ";
  // }
  // std::cout << "\n";
}