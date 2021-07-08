#include <initializer_list>
#include <tuple>
#include <utility>
#include <type_traits>
#include <iostream>

using namespace std;

enum class number {
    zero,
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    end
};

template <typename T>
bool is_in(const T& value, initializer_list<T> value_list) {
    for (const auto& item : value_list) {
        if (value == item) {
            return true;
        }
    }
    return false;
}

template <number n>
struct number_traits;

template <>
struct number_traits<number::zero> {
    static constexpr bool is_prime = false;
};

template <>
struct number_traits<number::one> {
    static constexpr bool is_prime = false;
};

template <>
struct number_traits<number::two> {
    static constexpr bool is_prime = true;
};

template <>
struct number_traits<number::three> {
    static constexpr bool is_prime = true;
};

template <>
struct number_traits<number::four> {
    static constexpr bool is_prime = false;
};

template <>
struct number_traits<number::five> {
    static constexpr bool is_prime = true;
};

template <>
struct number_traits<number::six> {
    static constexpr bool is_prime = false;
};

template <>
struct number_traits<number::seven> {
    static constexpr bool is_prime = true;
};

template <typename E, size_t... ints>
constexpr auto make_all_enum_consts_impl(index_sequence<ints...>) {
    return make_tuple(integral_constant<E, E(ints)>{}...);
}

template <typename E>
constexpr auto make_all_enum_consts() {
    return make_all_enum_consts_impl<E>(make_index_sequence<size_t(E::end)>{});
}

#define ENUM_FILTER_FROM(E, T, tup)                      \
    apply(                                          \
        [](auto... ts) {                                 \
            return tuple_cat(                       \
                conditional_t<                      \
                    E##_traits<decltype(ts)::value>::T,  \
                    tuple<decltype(ts)>,            \
                    tuple<>>{}...);                 \
        },                                               \
        tup)

constexpr bool is_prime(number n)
{
    return is_in(n,
                 make_values_from_consts(ENUM_FILTER_FROM(
                     number, is_prime,
                     make_all_enum_consts<number>())));
}

int main() {

    cout << is_prime(number::four) << endl;

    return 0;
}
