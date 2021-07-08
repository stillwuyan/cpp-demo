#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

void reserve_push(const std::string& file)
{
    std::ifstream reader(file);
    std::vector<char> buffer(1024*1024);
    size_t count = 0;
    while (reader)
    {
        reader.read(&buffer[0], buffer.size());
        if (reader.gcount() < buffer.size())
        {
            buffer.resize(reader.gcount());
        }
        count += std::count(buffer.begin(), buffer.end(), '\n');
    }

    std::vector<std::string> data(count);

    reader.clear();
    reader.seekg(0, std::ios_base::beg);
    std::string line;
    while (std::getline(reader, line))
    {
        data.emplace_back(line);
    }
    std::cout << "[reserve_push]finish reading, lines count: " << count << std::endl;
}

void reserve_push1(const std::string& file)
{
    std::ifstream reader(file);
    size_t count = 0;
    std::string line;
    while (std::getline(reader, line))
    {
        count++;
    }

    std::vector<std::string> data(count);

    reader.clear();
    reader.seekg(0, std::ios_base::beg);
    while (std::getline(reader, line))
    {
        data.emplace_back(line);
    }
    std::cout << "[reserve_push1]finish reading, lines count: " << count << std::endl;
}

void direct_push(const std::string& file)
{
    std::string line;
    std::ifstream reader(file);
    std::vector<std::string> data(30000);

    while (std::getline(reader, line))
    {
        data.emplace_back(line);
    }
    std::cout << "[direct_push]finish reading, lines count: " << data.size() << std::endl;
}

size_t use_getline(const std::string& file)
{
    std::string line;
    std::ifstream reader(file);
    size_t count = 0;
    while (std::getline(reader, line))
    {
        count++;
    }
    std::cout << "[use_getline]finish reading, lines count: " << count << std::endl;
    return count;
}

size_t use_buffiter(const std::string& file)
{
    std::ifstream reader(file);
    size_t count = std::count(std::istreambuf_iterator<char>(reader), std::istreambuf_iterator<char>(), '\n');
    std::cout << "[use_buffiter]finish reading, lines count: " << count << std::endl;
    return count;
}

size_t use_buffer(const std::string& file)
{
    std::ifstream reader(file);
    std::vector<char> buffer(1024*1024);
    size_t count = 0;
    while (reader)
    {
        reader.read(&buffer[0], buffer.size());
        if (reader.gcount() < buffer.size())
        {
            buffer.resize(reader.gcount());
        }
        count += std::count(buffer.begin(), buffer.end(), '\n');
    }
    std::cout << "[use_buffer]finish reading, lines count: " << count << std::endl;
    return count;
}

size_t use_buffer2(const std::string& file)
{
    std::ifstream reader(file);
    char buffer[1024*1024];
    size_t count = 0;
    size_t read_size = 0;
    while (reader)
    {
        reader.read(buffer, 1024*1024);
        read_size = reader.gcount();
        for (size_t i = 0; i < read_size; i++)
        {
            if (buffer[i] == '\n')
            {
                count++;
            }
        }
    }
    std::cout << "[use_buffer2]finish reading, lines count: " << count << std::endl;
    return count;
}
int main(int argc, char* argv[])
{
    if (argc == 3)
    {
        std::string file(argv[1]);
        int opt = atoi(argv[2]);
        switch (opt)
        {
        case 0:
            reserve_push(file);
            break;
        case 1:
            reserve_push1(file);
            break;
        case 2:
            direct_push(file);
            break;
        case 3:
            use_getline(file);
            break;
        case 4:
            use_buffiter(file);
            break;
        case 5:
            use_buffer(file);
            break;
        case 6:
            use_buffer2(file);
            break;
        default:
            break;
        }
    }
    else
    {
        std::cout << "parameter error, should be \"test {file name} {0|1|2|3}\"" << std::endl;
    }
    return 0;
}
