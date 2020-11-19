#include <iostream>
#include <cxxabi.h>

template <typename... Types>
class StrongType;

template <typename T>
class StrongType<T> {
public:
    typedef T Type;
};

template <typename A, typename B>
class StrongType<A, B> {
private:
    static A __A();
    static B __B();
public:
    typedef decltype(true?__A():__B()) Type;
};

template <typename T, typename... Types>
class StrongType<T, Types...> {
public:
    typedef typename StrongType<T, typename StrongType<Types...>::Type>::Type Type;
};

int main() {
    int status;
    char *realname = abi::__cxa_demangle(typeid(StrongType<int, double>::Type).name(), 0, 0, &status);
    std::cout << "(int, double) -> " << realname << std::endl;
    free(realname);

    realname = abi::__cxa_demangle(typeid(StrongType<char, int, float>::Type).name(), 0, 0, &status);
    std::cout << "(char, int, float) -> " << realname << std::endl;
    free(realname);
    
    realname = abi::__cxa_demangle(typeid(StrongType<char, int, float, double>::Type).name(), 0, 0, &status);
    std::cout << "(char, int, float, double) -> " << realname << std::endl;
    free(realname);
    return 0;
}