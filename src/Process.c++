#include "Process.h"
// #include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <string>
#include <iostream>

Process::Process(std::string filename)
{
    pid = vfork();
    if (pid == 0) { /*  process */
        execl(filename.c_str(), filename.c_str(), NULL); /* after a successful execl the parent should be resumed */
        _Exit(127); /* terminate the child in case execl fails */
    } else if (pid < 0) {
        std::cout << "Error running child process." << std::endl;
        exit(EXIT_FAILURE);
    }
}
Process::Process(std::string filename, int argc, const char* argv[])
{
    // TODO: Capture stdout and view in the app? http://www.microhowto.info/howto/capture_the_output_of_a_child_process_in_c.html
    // Just add first arg and sentinel
    const char* argv2[argc + 2];
    argv2[0] = filename.c_str();
    for (int i = 0; i < argc; i ++)
    {
        argv2[i + 1] = argv[i];
    }
    argv2[argc + 1] = NULL;


    pid = vfork();
    if (pid == 0) { /*  process */
        execv(filename.c_str(), (char**) argv2); /* after a successful execl the parent should be resumed */
        _Exit(127); /* terminate the child in case execl fails */
    } else if (pid < 0) {
        std::cout << "Error running child process." << std::endl;
        exit(EXIT_FAILURE);
    }
}

Process::~Process()
{
    if (pid > 0) {
        kill(pid, SIGTERM);
    }
}
