#pragma once
#include <string>

class Process
{
public:
    Process(std::string name);
    Process(std::string filename, int argc, const char* argv[]);

    pid_t GetPid() { return pid; }

    ~Process();
private:
    pid_t pid;

    void CreatePipe();
    void ClosePipeReadEnd();
    int communication_fd[2];
};
