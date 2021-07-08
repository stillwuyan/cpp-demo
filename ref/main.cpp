#include <iostream>
#include <string>

using namespace std;

// const lref
string f()
{
    return "abc";
}

void test_f()
{
    const string &s1 = f();
    cout << s1 << endl;

    // compile error: rvalue to non-const lref
    //string &s2 = f();
    //cout << s2 << endl;
}

// life cycle
class shape {
public:
    shape() { cout << "shape" << endl; }

    virtual ~shape() {
        cout << "~shape" << endl;
    }
};
class circle : public shape {
public:
    circle() { cout << "circle" << endl; }


    ~circle() {
        cout << "~circle" << endl;
    }
};
class triangle : public shape {
public:
    triangle() { cout << "triangle" << endl; }


    ~triangle() {
        cout << "~triangle" << endl;
    }
};
class rectangle : public shape {
public:
    rectangle() { cout << "rectangle" << endl; }

    ~rectangle() {
        cout << "~rectangle" << endl;
    }
};
class result {
public:
    result() { puts("result()"); }

    ~result() { puts("~result()"); }
};
result process_shape(const shape &shape1, const shape &shape2) {
    puts("process_shape()");
    return result();
}

void test_nodelay()
{
    process_shape(circle(), triangle());
}

void test_delay()
{
    result&& r = process_shape(circle(), triangle());
}

void test_move()
{
    result&& a = process_shape(circle(), triangle());
    result b = move(a); // move constructor
}

template<typename T>
void g(T &&param) {
    static_assert(std::is_lvalue_reference<T>::value, "T& is lvalue reference");
    cout << "T& is lvalue reference" << endl;
}

void test_type() {
    int x;
    int &&r1 = 10;
    int &r2 = x;
    g(r1);
    g(r2);
}

void test_rref() {
    auto&& x = 27;
    cout << "x is " << x << ",address is " << &x << endl;
    x = 12;
    cout << "x is " << x << ",address is " << &x << endl;
}

int main()
{
    //test_f();
    //test_nodelay();
    //test_delay();
    //test_move();
    //test_forward();
    test_rref();
    return 0;
}
