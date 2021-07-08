#include <memory>
#include <iostream>

using namespace std;

class TestOperator {
public:
    TestOperator() = default;
    ~TestOperator() = default;

    template <typename T>
    bool operator==(const T& v) {
        cout << "TestOperator::==(const T& v) executing" << endl;
        return true;
    }
};
template < typename T>
bool operator==(const T& v, const TestOperator& t) {
    cout << "operator==(const T& v, const TestOperator& t) executing" << endl;
    return true;
}
void test_operator() {
    TestOperator t;
    cout << "1 == t" << endl;
    1 == t;
    cout << "t == 1" << endl;
    t == 1;
}

void test_ptr_null() {
    cout << "p is not nullptr" << endl;
    unique_ptr<int> p = make_unique<int>(1);
    cout << "(nullptr != p) result: " << (nullptr != p) << endl;
    cout << "(p != nullptr) result: " << (p != nullptr) << endl;

    cout << "q is nullptr" << endl;
    unique_ptr<int> q;
    cout << "(nullptr != q) result: " << (nullptr != q) << endl;
    cout << "(q != nullptr) result: " << (q != nullptr) << endl;
}

int main() {
    //test_ptr_null();
    test_operator();
    return 0;
}
