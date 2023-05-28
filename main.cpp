#include <iostream>
#include <type_traits>
#include <vector>
#include <exception>

#include "KVector.hpp"

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
  long long* ptr;

  Kek() {
    // std::cout << "Kek()\n";
    ptr = new long long(5);
  }

  ~Kek() {
    // std::cout << "~Kek()\n";
    delete ptr;
  }
  Kek(const Kek& some_kek) {
    // std::cout << "Kek(const Kek&)\n";
    ptr = new long long(*(some_kek.ptr));
  }
  Kek(Kek&& some_kek) {
    // std::cout << "Kek(Kek&&)\n";
    ptr = some_kek.ptr;
    some_kek.ptr = nullptr;
  }
};

int main() {
  Kek kek;
  ink::KVector<Kek> kek_vec;
  try {
    kek_vec.push_back(kek);
  } catch (std::exception& e) {
    std::cout << e.what() << "\n";
  }
  kek_vec.push_back(std::move(kek));

  ink::KVector<int> vec;
  for (int i = 0; i < 5; ++i) {
    vec.push_back(i);
  }
  for (int i = 0; i < 5; ++i) {
    std::cout << vec[i] << " ";
  }
  std::cout << '\n';

  std::cout << vec.size() << " " << vec.empty() << "\n";

  ink::KVector<int> empty_vec_;
  std::cout << empty_vec_.empty() << " " << empty_vec_.size() << "\n";
}