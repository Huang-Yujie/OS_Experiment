#define _POSIX_SOURCE
#define MAX_MESSAGE_SIZE 1024

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int child1 (int[]);
int child2 (int[]);
void handlerParent(int sig); 
void handlerChild1(int sig);
void handlerChild2(int sig);

static pid_t pid1, pid2;

int main (int argc, char *argv[])
{
    signal (SIGINT, handlerParent);
    int pipefd1[2], pipefd2[2];
    pipe (pipefd1);
    pipe (pipefd2);
    if ((pid1 = fork()) == 0)
    {
        //child1
        signal(SIGINT, SIG_IGN);
        close (pipefd2[0]);
        close (pipefd2[1]);        
        // register the child 1 signal handler
        signal(SIGUSR1, handlerChild1);
        return child1(pipefd1);
    }
    if ((pid2 = fork()) == 0)
    {
        //child2
        signal(SIGINT, SIG_IGN);
        close (pipefd1[0]);
        close (pipefd1[1]);        
        // register the child 2 signal handler
        signal(SIGUSR1, handlerChild2);
        return child2(pipefd2);
    }
    close(pipefd1[0]);
    close(pipefd2[0]);
    char buffer[MAX_MESSAGE_SIZE];
    // run the main loop
    for (unsigned counterSend = 1; counterSend <= 5; counterSend++) 
    {
        // interval: 1 second
        sleep(1);
        // initialize and write message to the pipe
        memset(buffer, 0, MAX_MESSAGE_SIZE);
        sprintf(buffer, "I send child1 %u times", counterSend);
        write(pipefd1[1], buffer, strlen(buffer));
        memset(buffer, 0, MAX_MESSAGE_SIZE);
        sprintf(buffer, "I send child2 %u times", counterSend);
        write(pipefd2[1], buffer, strlen(buffer));
    }
    close(pipefd1[1]);
    close(pipefd2[1]);

    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    printf("Parent Process exit normally\n");

    return  0;
}

int child1 (int pipefd1[2])
{
    // initialize the buffer
    char buffer[MAX_MESSAGE_SIZE];
    // close unused pipe
    close(pipefd1[1]);

    ssize_t size;
    while (1) {
        memset(buffer, 0, MAX_MESSAGE_SIZE);
        size = read(pipefd1[0], buffer, MAX_MESSAGE_SIZE);
        // handle error
        if (size < 0)
            return -1;
        if (size == 0)
        {
            printf("Child1 Process exit normally\n");
            close(pipefd1[0]);
            exit(0);
        }
        // print the message read
        buffer[size] = 0;
        printf("%s\n", buffer);
    }
}

int child2 (int pipefd2[2])
{
    // initialize the buffer
    char buffer[MAX_MESSAGE_SIZE];
    // close unused pipe
    close(pipefd2[1]);

    ssize_t size;
    while (1) {
        memset(buffer, 0, MAX_MESSAGE_SIZE);
        size = read(pipefd2[0], buffer, MAX_MESSAGE_SIZE);
        // handle error
        if (size < 0)
            return -1;
        if (size == 0)
        {
            printf("Child2 Process exit normally\n");
            close(pipefd2[0]);
            exit(0);
        }
        // print the message read
        buffer[size] = 0;
        printf("%s\n", buffer);
    }
}
void handlerParent(int sig) 
{
    kill(pid1, SIGUSR1);
    kill(pid2, SIGUSR1);
    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    printf("Parent Process is Killed!\n");
    exit(0);
}

void handlerChild1(int sig) 
{
    printf("Child Process 1 is Killed by Parent!\n");
    exit(0);
}

void handlerChild2(int sig) 
{
    printf("Child Process 2 is Killed by Parent!\n");
    exit(0);
}