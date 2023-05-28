#include <gtest/gtest.h>
#include <string>

#include "KUnorderedMap.h"

struct KekStruct {
    KekStruct() : ival(0), sval("") {}
    KekStruct(int _ival, const std::string& _sval)
        : ival(_ival), sval(_sval) {}
    KekStruct(KekStruct&& other) noexcept :
        ival(other.ival), sval(other.sval) {

        std::cout << "KekStruct MOVE\n";
    }
    KekStruct& operator=(KekStruct&& other) noexcept {
        std::cout << "KekStruct MOVE operator=\n";
        ival = std::move(other.ival);
        sval = std::move(other.sval);
        return *this;
    }
    KekStruct(const KekStruct& other) :
        ival(other.ival), sval(other.sval) {

        std::cout << "KekStruct COPY\n";
    }
    KekStruct& operator=(const KekStruct& other) {
        std::cout << "KekStruct COPY operator=\n";
        ival = other.ival;
        sval = other.sval;
        return *this;
    }

    friend bool operator==(const KekStruct& lhs, const KekStruct& rhs) {
        return (lhs.ival == rhs.ival)
            || (lhs.sval == rhs.sval);
    }

    int ival;
    std::string sval;
};

struct KekHash {
    size_t operator()(const KekStruct& some_kek_struct) const {
        return std::hash<int>{}(some_kek_struct.ival);
    }
};


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
    std::vector<std::pair<int, int>> storage = { {1, 10} , {2, 20}, {3, 30} };
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