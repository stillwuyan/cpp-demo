#include <vector>
#include <string>

using namespace std;

int sum(vector<int> value)
{
    int result = 0;
    for (auto& v : value) result +=v;
    return result;
}
