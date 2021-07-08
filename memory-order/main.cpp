#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <atomic>
#include <chrono>

class Reader
{
private:
    std::atomic<size_t> _lines_no;
    std::vector<std::string> _lines;
    std::string _file;

public:
    Reader() : _lines_no(0), _file("/mnt/c/Data/Users/xu_q2/Tmp/cns/log/ota_install_succes_after_reboot.log")
    {
        Prefetch();
    }

    ~Reader() = default;

    const std::string& operator[](size_t index)
    {
        return _lines.at(index);
    }

    size_t LineNo()
    {
        return _lines_no.load(std::memory_order_acquire);
    }

    void Prefetch()
    {
        if (_lines_no > 0)
        {
            _lines.clear();
            _lines_no.store(0, std::memory_order_release);
        }

        std::thread t([this]{
            //std::cout << "reading " << _file << std::endl;
            std::ifstream reader(_file);
            std::string line;
            while (std::getline(reader, line))
            {
                //std::cout << "insert line: " << line << std::endl;
                _lines.emplace_back(line);
                _lines_no.fetch_add(1, std::memory_order_release);
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
        });
        t.detach();
    }
};

int main()
{
    Reader test{};
    for (int i = 0; i < 100; i++)
    {
        std::cout << "------------begin------------" << std::endl;
        size_t count = test.LineNo();
        std::cout << "line number: " << count << std::endl;
        for (int j = 0; j < count; j++)
        {
            std::cout << test[j].c_str() << std::endl;
        }
        std::cout << "------------end------------" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    return 0;
}
