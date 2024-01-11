#include <gtest/gtest.h>
#include <string>

#include "KVector.hpp"

struct NonDefaultConstr {
  NonDefaultConstr() = delete;
  explicit NonDefaultConstr(int new_num) : num(new_num) {}

  ~NonDefaultConstr() = default;

  NonDefaultConstr(const NonDefaultConstr&) = delete;
  NonDefaultConstr& operator=(const NonDefaultConstr&) = delete;

  NonDefaultConstr(NonDefaultConstr&&) = default;
  NonDefaultConstr& operator=(NonDefaultConstr&&) = default;

  int num;
};

struct Kek {
  int64_t* ptr{nullptr};

  Kek() : ptr(new std::int64_t(5)) {}

  ~Kek() { delete ptr; }

  Kek(const Kek& some_kek) {
    delete ptr;
    // NOLINTNEXTLINE (cppcoreguidelines-prefer-member-initializer)
    ptr = new int64_t(*(some_kek.ptr));
  }
  Kek& operator=(const Kek&) = delete;

  Kek(Kek&& some_kek) noexcept {
    delete ptr;
    // NOLINTNEXTLINE (cppcoreguidelines-prefer-member-initializer)
    ptr = some_kek.ptr;
    some_kek.ptr = nullptr;
  }
  Kek& operator=(Kek&&) = delete;
};

using size_type = ink::KVector<int>::size_type;

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

TEST_F(KVectorTest, Modifiers) {
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

  ink::KVector<NonDefaultConstr> std_vec;
  for (int i = 0; i < 3; ++i) {
    std_vec.emplace_back(i);
    EXPECT_EQ(std_vec[i].num, i);
  }
}

TEST_F(KVectorTest, Capacity) {
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

TEST_F(KVectorTest, Iterators) {
  EXPECT_EQ(empty_vec_.begin(), empty_vec_.end());
  EXPECT_EQ(empty_vec_.rbegin(), empty_vec_.rend());

  int value = 0;
  for (auto&& num : int_vec_) {
    EXPECT_EQ(num, value++);
  }

  auto it = int_vec_.begin();
  auto next_it = std::next(it, 2);
  EXPECT_TRUE(it < next_it);
  EXPECT_FALSE(it > next_it);
  EXPECT_TRUE(it <= next_it);
  EXPECT_FALSE(it >= next_it);
  EXPECT_FALSE(it == next_it);
  EXPECT_TRUE(it != next_it);

  ink::KVector deduction_vec(int_vec_.begin(), int_vec_.end());
  EXPECT_EQ(deduction_vec.size(), int_vec_.size());
  EXPECT_EQ(deduction_vec.capacity(), int_vec_.size());
}

TEST_F(KVectorTest, Constructors) {
  ink::KVector init_list_vec({0, 1, 2, 3, 4});
  for (size_type i = 0; i < int_vec_.size(); ++i) {
    EXPECT_EQ(init_list_vec[i], int_vec_[i]);
  }

  ink::KVector copy_int_vec(int_vec_);
  for (size_type i = 0; i < int_vec_.size(); ++i) {
    EXPECT_EQ(copy_int_vec[i], int_vec_[i]);
  }
  EXPECT_EQ(copy_int_vec.size(), int_vec_.size());

  ink::KVector tmp = copy_int_vec;
  ink::KVector move_int_vec(std::move(tmp));
  for (size_type i = 0; i < int_vec_.size(); ++i) {
    EXPECT_EQ(copy_int_vec[i], move_int_vec[i]);
  }
  EXPECT_EQ(copy_int_vec.size(), move_int_vec.size());
}
