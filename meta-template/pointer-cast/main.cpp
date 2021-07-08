#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

template <typename T>
struct AddStar {
    typedef T* Type;
};

template <typename T>
struct AddStar<T*> {
    typedef T* Type;
};

template <typename T>
struct RemoveStar {
    typedef T Type;
};

template <typename T>
struct RemoveStar<T*> {
    typedef typename RemoveStar<T>::Type Type;
};


int main(void) {
    int status;
    std::cout << "to pointer:" << std::endl;
    char *realname = abi::__cxa_demangle(typeid(AddStar<int>::Type).name(), 0, 0, &status);
    std::cout << "int -> " << realname << std::endl;
    free(realname);

    realname = abi::__cxa_demangle(typeid(AddStar<int*>::Type).name(), 0, 0, &status);
    std::cout << "int* -> " << realname << std::endl;
    free(realname);

    realname = abi::__cxa_demangle(typeid(AddStar<int**>::Type).name(), 0, 0, &status);
    std::cout << "int** -> " << realname << std::endl;
    free(realname);

    std::cout << "remove star:" << std::endl;
    realname = abi::__cxa_demangle(typeid(RemoveStar<int>::Type).name(), 0, 0, &status);
    std::cout << "int -> " << realname << std::endl;
    free(realname);

    realname = abi::__cxa_demangle(typeid(RemoveStar<int*>::Type).name(), 0, 0, &status);
    std::cout << "int* -> " << realname << std::endl;
    free(realname);

    realname = abi::__cxa_demangle(typeid(RemoveStar<int**>::Type).name(), 0, 0, &status);
    std::cout << "int** -> " << realname << std::endl;
    free(realname);

    return 0;
}
