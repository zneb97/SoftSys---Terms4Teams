/*
T4t Launcher
*/
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <argp.h>

int launch_server(){
    fprintf(stdout, "Launching T4T Server...\n");
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        // Child process
        if (system("./server") == -1) {
            fprintf(stderr, "./server Not Found\n");
        }
        exit(1);
    } 
    else if (pid < 0) {
        // Error forking
        return 1;
    } 
    else {
        return 0;
    }
}
int launch_client(char* ip_addr){
    fprintf(stdout, "Launching T4T Client...\nConnecting to %s\n", ip_addr);
    pid_t pid;
    pid = fork();
    char cmd[100];
    strcpy(cmd, "./client ");
    strcat(cmd, ip_addr); 
    if (pid == 0) {
        // Child process
        if (system(cmd) == -1) {
            fprintf(stderr, "./client Not Found\n");
        }
        return 0;
    } 
    else if (pid < 0) {
        // Error forking
        return 1;
    }
}
int main(int argc, char** argv){
    if (argc > 3 || argc == 1){
        fprintf(stderr, "[*] Usage:\n[1] ./t4t -s \n[2] ./t4t -c <ip_addr> \n");
        return -1;
    }
    char ch;
    int err;
    while((ch = getopt(argc, argv, "sc:")) != EOF){
        switch (ch){
            case('s'):
                if((err = launch_server()) == 1){
                    fprintf(stderr, "Error launching server");
                }
                break;
            case('c'):
                if((err=launch_client(optarg)) == 1){
                    fprintf(stderr, "Error launching client");
                }
                break;
            default:
                fprintf(stderr, "[*] Usage:\n[1] ./t4t -s \n[2] ./t4t -c <ip_addr> \n");
                return 1;
        }
    }
    return 0;
}