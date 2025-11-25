/*Heathershell - A simple UNIX shell written in C*/
/*
Copyright (C) 2025 Julian Franko

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#define ARG_LIMIT 10

#define ANSI_COLOR_RED     "\e[1;31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define PROMPT "hsh$ "

int read_input(char** cmd, char** args);
int execute_builtin(char* cmd, char** args);
int execute_cmd(char* cmd, char** args);
int exit_program(char* cmd, char** args);

int main() {

    char* cmd = malloc(sizeof(char)*32);
    char** args = malloc(sizeof(char*)*ARG_LIMIT);

    void sigint_handler(int sig) {
        printf("\n");
        exit_program(cmd, args);
        return;
    }

    signal(SIGINT, sigint_handler);

    /*Main Loop*/
    while(1)
    {
        printf(ANSI_COLOR_RED PROMPT ANSI_COLOR_RESET);
        if (read_input(&cmd, args) == -1) { return -1; };
        if (!(execute_builtin(cmd, args))) {
            execute_cmd(cmd, args);
        }
    }

    free(args);
    free(cmd);
    return 0;
}

int read_input(char** cmd, char** args) {
    char* buf = NULL;
    size_t bufsize = 255;

    if ((getline(&buf, &bufsize, stdin) == -1)) {
        printf("\nError reading input\n");
        return -1;
    };

    char* token = strtok(buf, " ");
    strcpy(*cmd, token);

    int i = 1;
    while (token != NULL) {
        char* next = strtok(NULL, " ");
        if (next == NULL) {
            break;
        }
        args[i] = malloc(255);
        strcpy(args[i], next);
        i++;
    }

    /*Strip Newline Char*/
    switch(i) {
        case 1:
            cmd[0][strlen(cmd[0])-1] = '\0';
            args[0] = NULL;
            break;
        default:
            //args[i][strlen(args[i])] = '\0';
            args[i-1][strlen(args[i-1])-1] = '\0';
            args[i] = NULL;

            args[0] = malloc(255);
            strcpy(args[0], *cmd);
            break;
    }

    free(buf);
}

int execute_cmd(char* cmd, char** args) {
    pid_t pid;

    pid = fork();
    if (pid == 0) {

        if (execvp(cmd, args) == -1) {
            printf("Failed to open program\n");
        };
    }
    else {
        int wstatus;
        wait(&wstatus);
    }
    return 0;
}

int execute_builtin(char* cmd, char** args) {

    if ((strcmp("exit", cmd)) == 0) {
        exit_program(cmd, args);
        return 1;
    }

    return 0;

}

/*Properly exit the program*/
int exit_program(char* cmd, char** args) {

    for (int i = 0; i < ARG_LIMIT; i++) {
        if (args[i] != NULL) {
            free(args[i]);
        }
    }

    free(cmd);
    free(args);
    exit(0);
}
