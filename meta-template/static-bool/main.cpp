#include <iostream>

template <int v>
struct Int2Type {
    enum {value = v};
};

template <class T, bool isPolymorphic>
class SomeContainer {
private:
    void DoSomething(T* obj, Int2Type<true>) {
        T* newObj = obj->Clone();
    }
    void DoSomething(T* obj, Int2Type<false>) {
        T* newObj = new T(*obj);
    }
public:
    void DoSomething(T* obj) {
        DoSomething(obj, Int2Type<isPolymorphic>());
    }
};

int main(void) {
    SomeContainer<int, true>test;
    int value = 0;
    test.DoSomething(&value);
    return 0;
}