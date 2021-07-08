#include <thread>
#include <mutex>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

class Reader
{
private:
    std::vector<std::string> _data;
    size_t _size;
    std::mutex _mtx;

public:
    Reader() = default;
    ~Reader() = default;

    void read()
    {
        std::lock_guard<std::mutex> lck(_mtx);
        if (_data.size() > 0)
        {
            std::cout << _data[0] << std::endl;
        }
    }

    void write()
    {
        std::thread t([this]()
        {
            for (int i = 0; i < 10; i++)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::lock_guard<std::mutex> lck(_mtx);
                _data.emplace_back("hello world");
                _size++;
            }
        });
        t.detach();
    }
};

int main()
{
    Reader reader;
    reader.write();
    for (int i = 0; i < 10; i++)
    {
        reader.read();
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }
    return 0;
}
