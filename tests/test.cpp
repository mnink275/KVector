#include <gtest/gtest.h>
#include <string>

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
  long long* ptr{nullptr};

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
  Kek(Kek&& some_kek) noexcept {
    // std::cout << "Kek(Kek&&)\n";
    delete ptr;
    ptr = some_kek.ptr;
    some_kek.ptr = nullptr;
  }
};

#if 1
// Test Fixture
class KVectorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    for (int i = 0; i < 5; ++i) {
      int_vec_.push_back(i);
      Kek kek;
      kek_copy_vec_.push_back(kek);
      kek_move_vec_.push_back(std::move(kek));
    }
  }

  ink::KVector<int> empty_vec_;
  ink::KVector<int> int_vec_;
  ink::KVector<Kek> kek_copy_vec_;
  ink::KVector<Kek> kek_move_vec_;
};

#endif

TEST_F(KVectorTest, pop_back) {
  EXPECT_TRUE(empty_vec_.empty());
  EXPECT_EQ(empty_vec_.size(), 0);

  ASSERT_EQ(int_vec_.size(), 5);
  for (int i = 0; i < 5; ++i) {
    int_vec_.pop_back();
  }
  EXPECT_EQ(int_vec_.size(), 0);

  ASSERT_EQ(kek_copy_vec_.size(), 5);
  for (int i = 0; i < 5; ++i) {
    kek_copy_vec_.pop_back();
  }
  EXPECT_EQ(kek_copy_vec_.size(), 0);

  ASSERT_EQ(kek_move_vec_.size(), 5);
  for (int i = 0; i < 5; ++i) {
    kek_move_vec_.pop_back();
  }
  EXPECT_EQ(kek_move_vec_.size(), 0);
}

TEST_F(KVectorTest, reserve_resize) {
  empty_vec_.reserve(5);
  EXPECT_TRUE(empty_vec_.empty());
  empty_vec_.resize(5);
  EXPECT_FALSE(empty_vec_.empty());
  EXPECT_EQ(empty_vec_.size(), 5);

  EXPECT_EQ(int_vec_.capacity(), 8);
  int_vec_.reserve(10);
  EXPECT_EQ(int_vec_.capacity(), 10);

  for (int i = 5; i < 10; ++i) {
    int_vec_.push_back(i);
  }
  EXPECT_EQ(int_vec_.capacity(), 10);

  int_vec_.resize(3);
  EXPECT_EQ(int_vec_.capacity(), 10);
  EXPECT_EQ(int_vec_.size(), 3);

  int_vec_.shrink_to_fit();
  EXPECT_EQ(int_vec_.capacity(), 3);
  EXPECT_EQ(int_vec_.size(), 3);
}

#if 0  // future tests
// find()
TEST(UMapMethod, find) {
  ink::KUnorderedMap<int, int> local_int_map;
  for (int i = -10; i < 10; ++i) {
    local_int_map[i] = i + 100;
  }
  EXPECT_EQ(local_int_map.find(5)->second, 105);
  EXPECT_EQ(local_int_map.find(20), local_int_map.end());
}

// erase()
TEST(UMapMethod, erase) {
  ink::KUnorderedMap<int, int> local_int_map;
  for (int i = -10; i < 10; ++i) {
    local_int_map[i] = i + 100;
  }
  EXPECT_EQ(local_int_map.find(5)->second, 105);
  local_int_map.erase(5);
  EXPECT_EQ(local_int_map.find(5), local_int_map.end());
}

// isEmpty()
TEST(UMapMethod, isEmpty) {
  ink::KUnorderedMap<int, int> local_int_map;
  for (int i = -10; i < 10; ++i) {
    local_int_map[i] = i + 100;
  }
  EXPECT_FALSE(local_int_map.isEmpty());
  local_int_map.clear();
  EXPECT_TRUE(local_int_map.isEmpty());
}

// // copy and move operator=()
// TEST(UMapMethod, CopyAndMoveAssignOperator) {
//     ink::KUnorderedMap<int, int> local_int_map;
//     for (int i = -10; i < 10; ++i) {
//         local_int_map[i] = i + 100;
//     }

//     ink::KUnorderedMap<int, int> local_int_map_copy;
//     local_int_map_copy = local_int_map;
//     EXPECT_TRUE(local_int_map == local_int_map_copy);

//     local_int_map_copy = std::move(local_int_map);
//     EXPECT_TRUE(local_int_map == local_int_map_copy);
// }

// maxLoadFactor()
TEST(UMapMethod, HashPolicy) {
  ink::KUnorderedMap<int, int> local_int_map;
  EXPECT_EQ(local_int_map.maxLoadFactor(), 2.0f);
  local_int_map.maxLoadFactor(5.0f);
  EXPECT_EQ(local_int_map.maxLoadFactor(), 5.0f);
}

// iterator and const_iterator
TEST(UMapMethod, Iterators) {
  ink::KUnorderedMap<int, int> iterator_test_map;
  for (int i = 0; i < 10; i++) {
    iterator_test_map[i] = i + 50;
  }
  auto it = iterator_test_map.begin();
  auto const_it = iterator_test_map.cbegin();
  for (size_t i = 0; i < 3; i++) {
    ++it;
    ++const_it;
  }
  EXPECT_EQ(it->second, 53);
  EXPECT_EQ(const_it->second, 53);

  auto begin_it = iterator_test_map.begin();
  (*begin_it).second = 125;
  EXPECT_EQ(iterator_test_map.begin()->second, 125);
  // (*const_it).second = 125 <--- CE

  for (size_t i = 0; i < 3; i++) {
    --it;
    --const_it;
  }
  EXPECT_EQ(it->second, 125);
  EXPECT_EQ(const_it->second, 125);
}

// Construction from InputIt
TEST(UMapInitialization, ConstructFromIterators) {
  std::vector<std::pair<int, int>> storage = {{1, 10}, {2, 20}, {3, 30}};
  ink::KUnorderedMap<int, int> local_int_map(storage.begin(), storage.end());
  EXPECT_EQ(local_int_map[2], 20);

  struct stateless {
    stateless& operator=(const stateless& rhs) { return *this; }
    bool operator==(const stateless& rhs) { return true; }
  };

#if 0
    class BadOutputIterator {
    public:
    	using iterator_category = std::output_iterator_tag;
    };

    BadOutputIterator out_it;

    // thanks for SFINAE
    ink::KUnorderedMap<stateless, stateless> test(out_it, out_it);
    // this is CE --------------------------------^^^^^^^^^^^^^^^
#endif
}
#endif