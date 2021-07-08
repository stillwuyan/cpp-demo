#include <iostream>

int main()
{
    try
    {
        int * p = new int[1000000000000ul];
    }
    catch (const std::bad_alloc& e)
    {
        std::cout << "allocate failed: " << e.what() << std::endl;
    }
    return 1;
}
