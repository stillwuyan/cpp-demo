#include <string>
#include <iostream>

using namespace std;

int main()
{
    string left("hello world");
    if (left == "hello world")
    {
        cout << "equal" << endl;
    }
    else
    {
        cout << "not equal" << endl;
    }
    return 0;
}
