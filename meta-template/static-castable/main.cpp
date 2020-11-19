#include <iostream>
#include <string>

template <typename A, typename B>
class IsCastable {
private:
    typedef char __True;
    typedef struct {char _[2];} __False;

    static A __A();
    static __True test(B);
    static __False test(...);

public:
    static constexpr bool value = (sizeof(test(__A())) == sizeof(__True));
};

int main(void) {
    std::cout << "string to int is " << IsCastable<std::string, int>::value << std::endl; 
    std::cout << "int to char is " << IsCastable<int, char>::value << std::endl; 
    std::cout << "float to int is " << IsCastable<float, int>::value << std::endl; 
    std::cout << "double to int is " << IsCastable<double, int>::value << std::endl; 
    return 0;
}
