#include <iostream>
#include <chrono>
#include <fstream>
#include <tuple>
#include <thread>
#include <chrono>
#include <cstring>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/resource.h>

template <typename T>
void loop_proc(T&& process_task)
{
    DIR* dir;
    dir = opendir("/proc");

    struct dirent* entry;
    struct dirent* task_entry;
    DIR* task_dir;
    char task_path[256] = {0};
    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
            memset(task_path, 0, 256);
            int count = snprintf(task_path, 255, "/proc/%s/task", entry->d_name);
            if ((task_dir = opendir(task_path)) == NULL) {
                //std::cout << "ERROR failed to open " << task_path << std::endl;
                continue;
            }
            while((task_entry = readdir(task_dir)) != NULL) {
                if (task_entry->d_name[0] == '.') {
                    continue;
                }
                snprintf(&task_path[count], 255-count, "/%s/stat", task_entry->d_name);
                process_task(task_path);
            }
            closedir(task_dir);
        }
    }

    closedir(dir);
}

enum ParseState: uint32_t {
    PID = 1,
    COMM = 2,
    STATE = 3,
    PPID = 4,
    PGRP = 5,
    MINFLT = 10,
    CMINFLT = 11,
    MAJFLT = 12,
    CMAJFLT = 13,
    UTIME = 14,
    STIME = 15,
    CUTIME = 16,
    CSTIME = 17,
    PRIORITY = 18,
    NICE = 19,
    VSIZE = 23,
    RSS = 24,
    PROCESSOR = 39,
    RT_PRIORITY = 40,
    POLICY = 41,
    IO_WAIT = 42,
    SKIP_PART = 99,
};

template <uint32_t n>
std::string skip_space(const std::string& text, uint32_t pos, uint32_t& skip)
{
    uint32_t space_no = 0;
    uint32_t end = 0;
    while(text[pos+end] != ' ' || (++space_no) < n) {
        end++;
    }
    skip = end;
    return "";
}

std::string find_space(const std::string& text, uint32_t pos, uint32_t& skip)
{
    uint32_t end = 0;
    while (text[pos+end] != ' ') {
        end++;
    }
    skip = end;
    return text.substr(pos, end+1); // include the last space, have exception if this is the last one.
}

std::string find_bracket(const std::string& text, uint32_t pos, uint32_t& skip)
{
    uint32_t end = 0;
    //pos++;          // skip the left bracket
    while (text[pos+end] != ')' || text[pos+end+1] != ' ') {
        end++;
    }
    skip = end+1;   // skip the left bracket
    return text.substr(pos, end+2);
}

std::tuple<ParseState, std::string (*)(const std::string&, uint32_t, uint32_t&)> parse_machine[] = {
    { ParseState::PID, find_space },
    { ParseState::COMM, find_bracket },
    { ParseState::STATE, find_space },
    { ParseState::SKIP_PART, skip_space<1> },
    { ParseState::PGRP, find_space },
    { ParseState::SKIP_PART, skip_space<4> },
    { ParseState::MINFLT, find_space },
    { ParseState::CMINFLT, find_space },
    { ParseState::MAJFLT, find_space },
    { ParseState::CMAJFLT, find_space },
    { ParseState::UTIME, find_space },
    { ParseState::STIME, find_space },
    { ParseState::CUTIME, find_space },
    { ParseState::CSTIME, find_space },
    { ParseState::PRIORITY, find_space },
    { ParseState::SKIP_PART, skip_space<4> },
    { ParseState::VSIZE, find_space },
    { ParseState::RSS, find_space },
    { ParseState::SKIP_PART, skip_space<14>} ,
    { ParseState::PROCESSOR, find_space },
    { ParseState::SKIP_PART, skip_space<2>} ,
    { ParseState::IO_WAIT, find_space }
};
constexpr const uint32_t parse_machine_size = sizeof(parse_machine)/sizeof(parse_machine[0]);

std::string parse_stat(const std::string& text)
{
    std::string::size_type count = text.length();
    uint32_t step = 0;
    uint32_t skip = 0;
    std::string output;
    for (uint32_t i = 0; i < count && step < parse_machine_size; i++) {
        output += std::get<1>(parse_machine[step])(text, i, skip);
        i += skip;
        step++;
    }
    output.pop_back();  // remove the last space
    return output;
}

bool g_stop = false;
void sig_handler(int sig_no)
{
    if (sig_no == SIGINT) {
        std::cout << "INFO receive SIGINT" << std::endl;
        g_stop = true;
    } else if (sig_no == SIGTERM) {
        std::cout << "INFO receive SIGTERM" << std::endl;
        g_stop = true;
    }

}

int main(int argc, char* argv[])
{
    int opt;
    int32_t sleep_interval = 1000;  // ms
    int32_t priority = 20;
    int32_t core_idx = -1;
    int32_t max_count = 500;
    while((opt = getopt(argc, argv, "p:t:c:n:")) != -1) {
        switch (opt) {
        case 'p':
            priority = atoi(optarg);
            break;
        case 't':
            sleep_interval = atoi(optarg);
            break;
        case 'c':
            core_idx = atoi(optarg);
            break;
        case 'n':
            max_count = atoi(optarg);
        default:
            break;
        }
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        std::cout << "WARN can't catch SIGINT" << std::endl;
    }
    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        std::cout << "WARN can't catch SIGTERM" << std::endl;
    }

    if (core_idx >= 0 && core_idx < 8) {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(core_idx, &mask);
        sched_setaffinity(0, sizeof(mask), &mask);
    }

    if (priority >= -100 && priority < 0) { // RT thread: -1 <-> -100
        sched_param param;
        param.sched_priority = (-1-priority);
        pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    } else if (priority >= 0 && priority < 40) {    // CFS thread: 0 <-> 39
        setpriority(PRIO_PROCESS, 0, priority-20);
    } else {
        std::cout << "ERROR priority value should be in [-99, 39]" << std::endl;
    }

    const char* suffix_list[] = {
        "_1.log",
        "_2.log",
        "_3.log",
        "_4.log",
        "_5.log"
    };
    constexpr const uint32_t suffix_list_size = sizeof(suffix_list)/sizeof(suffix_list[0]);

    std::string save_file("/run/tasks_stat");
    int index = 0;
    int line_no = 0;

    std::ifstream check_file;
    std::ofstream out;
    for (index = 0; index < suffix_list_size; index++) {
        check_file.open(save_file+suffix_list[index]);
        if (check_file.good()) {
            check_file.close();
        } else {
            out.open(save_file+suffix_list[index], std::fstream::out|std::fstream::binary);
            break;
        }
    }
    if (index >= suffix_list_size) {
        index = 0;
        out.open(save_file+suffix_list[index], std::fstream::out|std::fstream::binary);
    }

    timespec ts = {0,0};
    timespec ts_wall = {0,0};
    uint64_t timestamp_us = 0UL;
    uint64_t wall_us = 0UL;
    while (!g_stop) {
        //auto start = std::chrono::steady_clock::now();
        std::ifstream in;
        clock_gettime(CLOCK_REALTIME, &ts_wall);
        clock_gettime(CLOCK_MONOTONIC, &ts);
        timestamp_us = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
        wall_us = ts_wall.tv_sec * 1000000 + ts_wall.tv_nsec / 1000;
        out << 't' << timestamp_us << ' ' << wall_us << '\n';

        loop_proc([&in, &out](const char* task_stat){
            in.open(task_stat);
            if (in.is_open()) {
                std::string line;
                std::getline(in, line);
                std::string res = parse_stat(line);
                out << res << '\n';
            }
            in.close();
        });
        line_no++;
        if (line_no >= max_count) {
            out.close();
            index = (index + 1) % suffix_list_size;
            out.open(save_file+suffix_list[index], std::fstream::out|std::fstream::binary);
            line_no = 0;
        }

        //auto end = std::chrono::steady_clock::now();
        //std::chrono::duration<double> diff = end - start;
        static uint32_t log_count = 0;
        if (log_count == 0) {
            //std::cout << "INFO loop all the tasks in the proc use: " << diff.count() << "s\n";
            std::cout << "INFO loop 10 times" << std::endl;;
        }
        log_count = (log_count + 1) % 10;

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_interval));
    }
    out.close();

    return 0;
}
