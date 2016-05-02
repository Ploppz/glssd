#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
void daemonize()
{
        
    /* Our process ID and Session ID */
    pid_t pid, sid;
    
    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
            exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
            exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);
            
    /* Open any logs here */        
            
    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) exit(EXIT_FAILURE);
    
    /* Change the current working directory */
    if ((chdir("/")) < 0) exit(EXIT_FAILURE);
    
    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
   exit(EXIT_SUCCESS);
}
