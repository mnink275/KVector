#include <iostream>
#include <type_traits>
#include <vector>

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
  Kek() {
    std::cout << "Kek()\n";
    ptr = new long long(5);
  }
  Kek(const Kek& some_kek) {
    std::cout << "Kek(const Kek&)\n";
    ptr = new long long(*(some_kek.ptr));
  }
  long long* ptr;
  ~Kek() {
    std::cout << "~Kek()\n";
    delete ptr;
  }
};


int main() {
  Kek kek;
  ink::KVector<Kek> kek_vec;
  kek_vec.push_back(kek);

  ink::KVector<int> vec;
  for (int i = 0; i < 5; ++i) {
    vec.push_back(i);
  }
  for (int i = 0; i < 5; ++i) {
    std::cout << vec[i] << " ";
  }
  std::cout << '\n';
}