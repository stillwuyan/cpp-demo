#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <atomic>

void multi_thread(int argc, const char* argv[]) {
    if (argc < 2) {
        printf("Error parameter!\n");
        printf("Usage: %s command\n", argv[0]);
        return;
    }
    std::string cmd;
    cmd += argv[1];
    for (int i = 2; i < argc; i++) {
        cmd += " ";
        cmd += argv[i];
    }

    int thread_count = 1;
    std::thread t[thread_count];

    for (int j = 0; j < thread_count; j++) {
        t[j] = std::thread([j, &cmd]() {
            int count = 0;
            do {
                std::cout << j << ":execute command: " << cmd << std::endl;
                FILE* cmdPipe = popen(cmd.c_str(), "r");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                int fd = fileno(cmdPipe);
                int flags;
                flags = fcntl(fd, F_GETFL, 0);
                flags |= O_NONBLOCK;
                fcntl(fd, F_SETFL, flags);

                std::string ret;
                if (NULL != cmdPipe) {
                    char readBuffer[512] = {'\0'};

                    std::cout << j << ":read pipe result" << std::endl;
                    while (fgets(readBuffer,
                                 static_cast<int32_t>(sizeof(readBuffer)) - 1,
                                 cmdPipe)) {
                        std::cout << j << ":read out: " << readBuffer
                                  << std::endl;
                        ret += readBuffer;
                    }

                    std::cout << j << ":resutl: " << ret << std::endl;
                    ret = ret.substr(0, ret.length() - 1);

                    pclose(cmdPipe);
                }
                count++;
            } while (count < 1);
        });
    }

    for (int j = 0; j < thread_count; j++) {
        t[j].join();
    }
}

void my_popen(const std::string& cmd, const std::string& name) {
    FILE* cmdPipe = ::popen(cmd.c_str(), "r");

    std::string ret;
    if (NULL != cmdPipe) {
        char readBuffer[512] = {'\0'};

        std::cout << name << " read pipe result" << std::endl;
        while (fgets(readBuffer,
                    static_cast<int32_t>(sizeof(readBuffer)) - 1,
                    cmdPipe)) {
            std::cout << name << " read out: " << readBuffer << std::endl;
            ret += readBuffer;
        }

        std::cout << name << " resutl: " << ret << std::endl;

        pclose(cmdPipe);
    } else {
        std::cout << name << " popen failed" << std::endl;
    }
}

void multi_process(int argc, const char* argv[]) {
    std::atomic<bool> stop {false};
    std::atomic<uint16_t> cnt {0};
    std::thread t1([&stop, &cnt](){
        while(!stop) {
            my_popen("ls /sys/bus/usb/devices/2-1.1.1:1.0/net/", "[main thread]");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << "[main thread]" << "current fork cnt: " << cnt << std::endl;
        };
    });

    while(true) {
        std::cout << "start forking" << std::endl;
        int fd = fork();
        if (fd < 0) {
            std::cout << "fork failed" << std::endl;
        } else if (fd == 0) {
            // Or it will clone the pipe fds of the main thread,
            // then the main thread will read nothing from the cloned pipe.
            //for(int fd=3; fd<256; fd++) (void) close(fd);

            // May be locked by proc_file_chain_lock,
            // because it is cloned from main thread and its state is in locking.
            my_popen("ls /sys/bus/usb/devices/2-1.1.1:1.0/net/", "[child process]");
            break;
        } else {
            cnt++;
            waitpid(fd, NULL, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    stop = true;
    t1.join();
}

int main(int argc, const char* argv[]) {
    multi_process(argc, argv);
    return 0;
}

