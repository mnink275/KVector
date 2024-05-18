#include <concepts>
#include <functional>
#include <type_traits>
#include <vector>

template <class T>
concept Hashable = requires(T t) {
  { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

template <class T>
requires(Hashable<T>)
void foo() {  }

int main() {
}
