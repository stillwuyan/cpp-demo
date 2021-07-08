#include <iostream>
#include <string>

using namespace std;

void output();

int main()
{
    string msg;

#if defined(USE_BM)
    msg = "using BM";
#elif defined(USE_MA)
    msg = "using MA";
#endif

    cout << "it is " << msg << endl;
    output();
    return 0;
}
