#include <memory>

template <class T>
class SingletonImpl {
public:
    static T& getInstance() {
        return *staticInstance();
    }

protected:
    SingletonImpl() = default;
    virtual ~SingletonImpl() = default;

private:
    SingletonImpl(const SingletonImpl&) = delete;
    SingletonImpl& operator=(const SingletonImpl&) = delete;

    static std::unique_ptr<T>& staticInstance() {
        static auto instance = std::unique_ptr<T>(new T);
        return instance;
    }
};
