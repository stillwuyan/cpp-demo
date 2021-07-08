#include <iostream>
#include <string>

int main()
{
    char a = 0x01;
    std::cout << "0x01 output is: " << a << std::endl;
    std::cout << "0x01 to int is: " << std::to_string(a) << std::endl;
    std::cout << 42 << std::endl;
    std::cout << std::hex << 42 << std::endl;
    std::cout << 42 << std::endl;

    char b = 0;
    b = '0';
    if (b == 48)
    {
        std::cout << "equal" << std::endl;
    }
    return 0;
}

