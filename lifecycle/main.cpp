#include <iostream>

class A {
public:
    A(int index):m_index(index) {
        std::cout << "A construct: " << m_index << std::endl;
    }
    ~A() {
        std::cout << "A destruct: " << m_index << std::endl;
    }
private:
    int m_index;
};

class B {
public:
    B(int index):m_index(index) {
        std::cout << "B construct: " << m_index << std::endl;
    }
    B(B&& b):m_index(0) {
        std::cout << "B move construct: " << m_index << std::endl;
    }
    ~B() {
        std::cout << "B destruct: " << m_index << std::endl;
    }
private:
    int m_index;
};

int main() {
    A a1(1);
    {
        B(100);
        B b1(1);
        B&& b0 = std::move(b1);
        B b2(2);
        A a2(2);
    }
    A a3(3);
    return 0;
}
