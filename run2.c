#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv)
{
    pid_t child;    
    int exitStatus;

    // at least, there should be 3 arguments
    // 2 for the first command, and the rest for the second command
    if (argc < 4) {
        fprintf(stderr, "Usage: %s cmd1 cmd1_arg cmd2 [cmd2_args ..]\n", argv[0]);
        return 1;
    }
    
    // TODO
    
    child = fork();
    //printf("Child, pid: %d\n", child);
    if (child == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    }
    else if (child == 0)
    {
        //char * first[] = {"./run2", argv[1], argv[2], NULL};
        //char * temp = argv[3];
        //printf("Running: %s %s\n", argv[1], argv[2]);
        //char ** temp = &(*argv) + 3;
        //printf("Running: %s %s %s\n", *(temp + 1), temp[1], temp[2]);
        execlp(argv[1], argv[1], argv[2], NULL);                    // Run the first command    
    }
    
    waitpid(child, &exitStatus, 0);
    
    
    printf("exited=%d exitstatus=%d\n", WIFEXITED(exitStatus), WEXITSTATUS(exitStatus));     
    pid_t secondChild = fork();
    if (child == -1)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    }
    else if (secondChild == 0)
    {
        char ** temp = &(*argv) + 3;
        //printf("Current arguments: %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
        //printf("Running: %s %s %s\n", *(temp), temp[1], temp[2]);
        execvp(*(temp), temp);                            // Run the second command
    }
    waitpid(secondChild, &exitStatus, 0);
    printf("exited=%d exitstatus=%d\n", WIFEXITED(exitStatus), WEXITSTATUS(exitStatus));
    
    return 0;
}