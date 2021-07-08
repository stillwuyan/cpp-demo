#include <string>
#include <iostream>

using namespace std;

class A
{
private:
    string name;
public:
    A() : name()
    {
        cout << "construct A { name: \"" << name << "\" } " << endl;
    }
    ~A() = default;
};

int main()
{
    A a;
    return 0;
}
