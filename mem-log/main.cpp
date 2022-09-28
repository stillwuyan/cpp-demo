#include <iostream>
#include <memory>
#include <algorithm>
#include <string>
#include <regex>
#include <thread>
#include <chrono>

#include <fstream> 

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<signal.h>

class IPipeline {
public:
    IPipeline() = default;
    virtual ~IPipeline() = default;

    virtual int output(const std::string& filter) {
        if (!_init) {
            return -1;
        }

        static const uint32_t len = 4096;
        char buffer[len] = {0};
        uint32_t offset = 0;
        int ret = 0;
        while (true) {
            ret = read(&buffer[offset], len-offset);
            if (ret < 0) {
                return -1;
            }

            std::string text(buffer);
            auto eol_pos = text.rfind('\n');
            search(text, eol_pos, filter);

            memset(buffer, 0, len);
            if (eol_pos != std::string::npos) {
                offset = len - eol_pos - 1;
                memcpy(buffer, &text[eol_pos+1], offset);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return 0;
    }

protected:
    virtual int read(char* buffer, int size) = 0;
    virtual int search(const std::string& text, std::string::size_type eol_pos, const std::string& filter) {
        auto begin = std::cbegin(text);
        auto end = eol_pos != std::string::npos ? (std::cbegin(text) + eol_pos + 1) : std::cend(text);
        std::string::size_type offset = 0;

        auto const regex = std::regex(filter);
        auto searchResults = std::smatch{};
        while (std::regex_search((begin+offset), end, searchResults, regex)) {
            auto pos = searchResults.prefix().length() + offset;

            auto sub_begin = text.rfind('\n', pos);
            sub_begin = sub_begin == std::string::npos ? 0 : sub_begin + 1;
            auto sub_end = text.find('\n', pos);
            sub_end = sub_end == std::string::npos ? text.length() : sub_end + 1;
            offset = sub_end;

            write(text.substr(sub_begin, sub_end-sub_begin));
        }
        return 0;
    }
    virtual int write(const std::string& line) = 0;

    bool _init = false;;
};

class NetSource: public IPipeline {
public:
    NetSource(std::string&& file): _file(file) {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd == -1) {
            std::cout << "ERROR opening socket" << std::endl;
            return;
        }

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(15361);
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            std::cout << "ERROR invaild address" << std::endl;
            return;
        }

        if (connect(_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
            std::cout << "ERROR connection failed" << std::endl;
            return;
        }

        std::ifstream check_file;
        for (_index = 0; _index < 5; _index++) {
            check_file.open(_file+_suffix_list[_index]);
            if (check_file.good()) {
                check_file.close();
            } else {
                _save.open(_file+_suffix_list[_index], std::fstream::out|std::fstream::binary);
                break;
            }
        }
        if (_index == 5) {
            _index = 0;
            _save.open(_file+_suffix_list[_index], std::fstream::out|std::fstream::binary);
        }

        if (!_save.is_open()) {
            std::cout << "ERROR open file failed" << std::endl;
            return;
        }

        if (signal(SIGINT, sig_handler) == SIG_ERR) {
            std::cout << "WARN can't catch SIGINT" << std::endl;
        }

        _line_no = 0;
        _init = true;
    }
    virtual ~NetSource() {
        if (_sockfd != -1) {
            close(_sockfd);
        }
        _save.flush();
        _save.close();
    }

    static void sig_handler(int signo) {
        if (signo == SIGINT) {
            std::cout << "receive SIGINT" << std::endl;
            _stop = true;
        }
    }

private:
    int read(char* buffer, int size) override {
        int offset = 0;
        int count = 0;
        do {
            count = ::read(_sockfd, &buffer[offset], size-offset);
            if (count <= 0) {
                std::cout << "ERROR read failed" << std::endl;
                return -1;
            }
            offset += count;
        } while (offset < size);

        if (_stop) {
            return -1;
        }

        return offset;
    }
    int write(const std::string& line) override {
        static const int max_count = 50000;
        _save.write(&line[0], line.size());
        _line_no++;
        if (_line_no >= max_count) {
            std::cout << "INFO reach to the maximum lines, need open new file " << _index << std::endl;
            _save.flush();
            _save.close();
            _index = (_index + 1) % 5;
            std::cout << "INFO open " << _file+_suffix_list[_index] << std::endl;
            _save.open(_file+_suffix_list[_index], std::fstream::out|std::fstream::binary);
        }
    }

    const char* _suffix_list[5] = {
        "_1.log",
        "_2.log",
        "_3.log",
        "_4.log",
        "_5.log"
    };
    int _index;
    int _sockfd;
    int _line_no;
    const std::string _file;
    std::fstream _save;

    static bool _stop;
};

class FileSource: public IPipeline {
public:
    FileSource(std::string&& file) {
        _source.open("log.txt", std::fstream::in|std::fstream::binary);

        _save.open(file, std::fstream::out|std::fstream::binary);
        if (!_save.is_open()) {
            std::cout << "ERROR open file failed" << std::endl;
            return;
        }

        _init = true;
    }
    virtual ~FileSource() {
        _source.close();
        _save.flush();
        _save.close();
    }

private:
    int read(char* buffer, int size) override {
        _source.read(buffer, size);
        if (_source.gcount() < size) {
            std::cout << "ERROR read only " << _source.gcount() << std::endl;
            return -1;
        } else {
            return size;
        }
    }
    int write(const std::string& line) override {
        _save.write(&line[0], line.size());
    }

    int _index;
    int _line_no;
    std::fstream _source;
    std::fstream _save;
};

bool NetSource::_stop = false;
int main(int argc, char* argv[]) {
    //std::unique_ptr<IPipeline> pipeline = std::make_unique<FileSource>("./output.txt");
    std::unique_ptr<IPipeline> pipeline = std::make_unique<NetSource>("/run/displaymanager");
    pipeline->output(R"(/displaymanagement|\[displaymanager|\[user\] weston)");

    return 0;
}
