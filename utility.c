//
// Created by Aidan Kubala on 10/18/22.
//
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "utility.h"

int quit(){
    exit(1);
}

int cd(char *dir){
    return chdir(dir);
}


void clr(){
    printf("\e[1;1H\e[2J");
}

void dir(char* dir){
    DIR *stream = opendir(dir);
    while (readdir(stream) != NULL){
        printf("%s\n", readdir(stream)->d_name);
    }
}

void enterpause(){
    while(fgetc(stdin) != '\n'){
        ;
    }
}

char* environname(char* name){
    return getenv(name);
}

void help(){
    size_t linesize = 1000;
    char *buffer;
    const char *userman = "userman.txt";
    FILE *man = fopen(userman, "r");
    while(getline(&buffer, &linesize, man) != -1){
        getline(&buffer, &linesize, man);
        printf("%s\n", buffer);
    }

}

int checkbuiltins(char* arg){
    int i;
    const char *builtins[] = {"cd", "clr", "dir", "environ", "echo", "help", "pause", "quit"};
    for (i=0;i<8;i++){
        if (strcmp(arg,builtins[i])){
            return i;
        }
    }
    return -1;
}

