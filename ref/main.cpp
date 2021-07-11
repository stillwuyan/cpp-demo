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

class A {
public:
    A():m_ptr(new int(0)) {
        std::cout << "default construct" << std::endl;
    }
    A(const A& a):m_ptr(new int(*a.m_ptr)) {
        std::cout << "copy construct" << std::endl;
    }
    A(A&& a):m_ptr(a.m_ptr) {
        a.m_ptr = nullptr;
        std::cout << "move construct" << std::endl;
    }
    ~A() {
        if (m_ptr) {
            delete m_ptr;
            m_ptr = nullptr;
        }
        std::cout << "destruct" << std::endl;
    }
private:
    int* m_ptr;
};

A GetA() {
    return A();
}

void test_rvo() {
    A a = GetA();
    A a2 = A(GetA());
    A a3 = std::move(a);
}

void test_array() {
    char a[10] = {0};
    char&& b = std::move(a[0]);

    std::cout << "a[0] address is " << std::hex << static_cast<void*>(&a[0]) << std::endl;
    std::cout << "b address is " << static_cast<void*>(&b) << std::endl;
}

int main()
{
    //test_f();
    //test_nodelay();
    //test_delay();
    //test_move();
    //test_forward();
    //test_rref();
    //test_rvo();
    test_array();
    return 0;
}
