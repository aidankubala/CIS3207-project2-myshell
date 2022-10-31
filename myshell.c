#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "utility.h"
#include <sys/wait.h>


int main() {
    char* line;
    char* cmd;
    char* buf;
    char* args[] = {};
    size_t pathsize = 2000;
    int background = 0;
    const char *n1 = "/myshell";
    const char *shell = "shell";
    const char delim = ' ';
    const char *bashargs[] = {"<", ">>", "|", ">", "&"};
    const char *builtins[] = {"cd", "clr", "dir", "environ", "echo", "help", "pause", "quit"};
    char end = '\0';
    int count = 0;
    size_t linesize = 80;
    perror(NULL);
    char* n = getcwd(buf, pathsize);
    perror(NULL);
    n = realloc(n, sizeof(n1));
    strcat(n,n1);
    setenv(shell, n, 0);
    //tokenize and parse user input
    printf("myshell>");
    getline(&line, &linesize, stdin);
    //save first argument passed as the command
    cmd = strtok(line, &delim);
    printf("%s\n", cmd);
    //the rest go into an array of args
    while (strtok(NULL, &delim) != NULL){
        args[count] = strtok(NULL, &delim);
        int i;
        for (i=0; i < sizeof(bashargs); i++){
            if (strcmp(args[count], bashargs[i]) == 0){
                if (i==0){ // <
                    // assigning the next value in the line to be the input location
                    FILE *input = fopen(args[count+1], "r");
                    int fd = fileno(input);
                    dup2(fileno(stdin), fd);
                    close(fd);
                }
                else if(i==1){ // >>
                    //assigning the next value in the line to the output location in appending mode
                    FILE *output = fopen(args[count+1], "a");
                    int fd = fileno(output);
                    dup2(fileno(stdout), fd);
                    close(fd);
                }
                else if(i==2){ // |
                    //piping
                    int pipefd[2];
                    FILE *first = fopen(cmd, "r+");
                    FILE *second = fopen(args[count+1], "r+");
                    pipefd[0] = fileno(first);
                    pipefd[1] = fileno(second);
                    pipe(pipefd);
                }
                else if(i==3){ // >
                    //assigning the next value in the line to be the output location
                    FILE *output = fopen(args[count+1], "w");
                    int fd = fileno(output);
                    dup2(fileno(stdout), fd);
                    close(fd);
                }
                else if(i==4){ // &
                    int g = fork();
                    background = 1;
                    if (g == 0) {
                        execv(args[count + 1], &args[count + 2]);
                        printf("ERROR: could not execute.\n");
                        exit(1);
                    }

                }
            }
        }
        count++;
    }
    args[count] = &end;
    int p;
    for (p=0;p<sizeof(builtins);p++){
        if (strcmp(cmd,builtins[p]) == 0){
            if (p==0){ //cd
                if (args[0] != NULL){
                    cd(args[0]);
                }
                else {
                    char cd[200];
                    getcwd(cd,200);
                    return atoi(cd);
                }
            }
            else if (p==1){ //clr
                printf("hi\n");
                clr();
            }
            else if (p==2){ //chdir
                dir(args[0]);
            }
            else if (p==3){ //environ
                int x;
                for (x=0;x<sizeof(args);x++){
                    environ(args[x]);
                }
            }
            else if (p==4){ //echo
                int x;
                for (x=0;x<sizeof(args);x++){
                    echo(args[x]);
                }
            }
            else if (p==5){ //help
                help();
            }
            else if (p==6){ //pause
                enterpause();
            }
            else if (p==7){ //quit
                quit();
            }
        }
    }
    //forking and execing the command
    int val = fork();
    if (background == 0){
        wait (0);
    }
    if (val == 0){
        execv(cmd, args);
        printf("ERROR: could not execute.\n");
        exit(1);
    }
    else {
        int pid = val;
        wait(&pid);
    }
    return 0;
}
