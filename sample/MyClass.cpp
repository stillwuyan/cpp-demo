#include "SingletonImpl.hpp"
#include "iostream"

class MyClass : public SingletonImpl<MyClass> {
public:
    ~MyClass() = default;
    void test() {
        std::cout << "hello world" << std::endl;
    }
private:
    MyClass() = default;
    MyClass(const MyClass&) = delete;
    MyClass& operator=(const MyClass&) = delete;
    friend class SingletonImpl<MyClass>;
};

int main() {
    MyClass::getInstance().test();
    //MyClass* p = new MyClass();
    return 0;
}
