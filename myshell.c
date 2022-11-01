#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utility.h"
#include <sys/wait.h>


int main(int argc, char* argv[]) {
    extern char **environ;
    char* line = NULL;
    char* cmd;
    char* args[20] = {};
    *args = (char*)malloc(800000);
    size_t pathsize = 200;
    int background = 0;
    const char *shell = "shell";
    const char delim = ' ';
    const char *bashargs[] = {"<", ">>", "|", ">", "&"};
    char end = '\0';
    int count = 0;
    char* token;
    size_t linesize = 80;
    
    char *value;
    value = getcwd(value, pathsize);
    setenv(shell, value, 1);
    
    if (argc!=1){
        int pid = fork();
        if (pid == 0){
            execvp(argv[0],&argv[1]);
        }
    }
    //tokenize and parse user input
    printf("myshell>");
    getline(&line, &linesize, stdin);
    //save first argument passed as the command
    token = strtok(line, &delim);
    cmd = token;
    token = strtok(NULL, &delim);
    while (token != NULL) {
        args[count] = token;
        token = strtok(NULL, &delim);
        printf("%s\n", token);
        printf("%d  %s\n", count, args[count]);
        count++;
    }
    count++;
    args[count] = '\0';
    printf("hi");
    // check for built-in commands and execute if so
    int check = checkbuiltins(cmd);
    if (check != -1){
        if (check == 0){ //cd
            if (args[0] != NULL){
                cd(args[0]);
            }
            else {
                char *cd;
                cd = malloc(200);
                getcwd(cd,200);
                printf("%s\n", cd);
            }
        }
        else if (check == 1){ //clr
            clr();
        }
        else if (check == 2){ //dir
            if (args[0] != NULL){
                dir(args[0]);
            }
            else {
                char *cd;
                cd = malloc(200);
                getcwd(cd,200);
                dir(cd);
            }
        }
        else if (check == 3){ //environ
            int x;
            for (x=0;x<count;x++){
                environname(args[x]);
            }
        }
        else if (check == 4){ //echo
            int x;
            for (x=0;x<count;x++){
                printf("%s ", args[x]);
            }
        }
        else if (check == 5){ //help
            help();
        }
        else if (check == 6){ //pause
            enterpause();
        }
        else if (check == 7){ //quit
            quit();
        }
    }
    else{

    }
    printf("yas");
    int z;
    for (z=0;z<count;z++){
        int i;
        for (i=0; i < sizeof(bashargs); i++){
            if (strcmp(args[z], bashargs[i]) == 0){
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
                        execvp("myshell", &args[i]);
                        printf("ERROR: could not execute.\n");
                        exit(1);
                    }

                }
            }
        }
    }
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
    args[count] = &end;
    //forking and execing the command
    int val = fork();
    if (background == 0){
        wait (0);
    }
    if (val == 0){
        execvp(cmd, args);
        printf("ERROR: could not execute.\n");
        exit(1);
    }
    else {
        int pid = val;
        wait(&pid);
    }
    return 0;
}
