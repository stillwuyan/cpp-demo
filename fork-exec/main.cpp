#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

bool exec4Pipe(char* const cmd[], char* const env[], int prepipe[2], int postpipe[2]) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cout << "Failed in fork" << &std::endl;
        return false;
    } else if (pid != 0) {
        pid_t ret;
        int status;

        if (prepipe) {
            close(prepipe[0]);
        }
        if (postpipe) {
            close(postpipe[1]);
        }

        while ((ret = waitpid(pid, &status, 0)) == -1) {
            if (errno != EINTR) {
                break;
            }
        }
        if ((ret == 0) || !(WIFEXITED(status) && !WEXITSTATUS(status))) {
            std::cout << "Failed in child process" << &std::endl;
            return false;
        }
    } else {
        if (prepipe) {
            dup2(prepipe[0], 0);
        }
        if (postpipe) {
            dup2(postpipe[1], 1);
            close(postpipe[0]);
            close(postpipe[1]);
        }

        execve(cmd[0], &cmd[1], env);
        std::cout << "Failed in execve" << &std::endl;
        _exit(127);
    }
    return true;
}

int main(int argc, char *argv[]) {
    char* const env[] = {NULL};
    char* const cmd_step1[] = {
        const_cast<char*>("/bin/gzip"),
        const_cast<char*>("gzip"),
        const_cast<char*>("-cd"),
        const_cast<char*>("/srv/data/Joynext/xu_q2/workspace/demo/cpp-demo/fork-exec/test.txt.gz"),
        nullptr
    };
    char* const cmd_step2[] = {
        const_cast<char*>("/bin/grep"),
        const_cast<char*>("grep"),
        const_cast<char*>("hello\\|sizzle"),
        nullptr
    };

    int pipe_step1[2];
    if (pipe(pipe_step1) == -1) {
        std::cout << "Failed to create pipe step1" << &std::endl;
        return -1;
    }

    if (!exec4Pipe(cmd_step1, env, nullptr, pipe_step1)) {
        std::cout << "Failed in cmd step1" << &std::endl;
        close(pipe_step1[0]);
        return -1;
    }

    int pipe_step2[2];
    if (pipe(pipe_step2) == -1) {
        std::cout << "Failed to create pipe step2" << &std::endl;
        return -1;
    }

    if (!exec4Pipe(cmd_step2, env, pipe_step1, pipe_step2)) {
        std::cout << "Failed in cmd step2" << &std::endl;
        close(pipe_step2[0]);
        return -1;
    }

    std::ofstream f("./result.txt");
    char buf[16] = {0};
    while (read(pipe_step2[0], buf, 15) > 0) {
        f << buf;
        memset(buf, 0, 16);
    }
    close(pipe_step2[0]);
    f.close();

    return 0;
}
