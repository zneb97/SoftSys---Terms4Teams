// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

//Libraries we added
#include <time.h>
#include <ncurses.h>


#define PORT 8000
#define BUF_SIZE 10000

#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif

WINDOW *win;

void * receiveMessage(void * socket) {
    int sockfd, ret;
    char buffer[BUF_SIZE];
    sockfd = (int) socket;
   
    memset(&buffer, 0, sizeof(buffer));
    while(1){
        ret = recv(sockfd , buffer, BUF_SIZE, 0);
        //Could not make connection
        if(ret == 0){
            wprintw(win, "Server not available! Please Ctrl-D");
            pthread_exit(NULL);
            exit(0);
        }
        //Problem with data transfer
        else if (ret < 0) {
            wprintw(win, "Error receiving data!\n");
        }
        //Received data, update window
        else {
            wprintw(win, buffer);
        }
    }
    pthread_exit(NULL);
   
}

int main(int argc, char const *argv[]){
    // Init Ncurses + Window
    initscr();
    int row;
    int col;
    getmaxyx(stdscr, row, col);	
    win = newwin(row - 2, col - 2, 1, 1);
    scrollok(win,TRUE);
    raw();
    keypad(stdscr, TRUE);
    noecho();
    wrefresh(win);

    // Init char position
    char ch;
    int b_pos = 0;

    // Init Network Stuff
    struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char username[BUF_SIZE];
    char buffer[BUF_SIZE] = {0};
    char prev_buffer[BUF_SIZE] = {0};
    const char * serverAddr;
    pthread_t rThread;

    if (argc <2) { //checks that user input server ip address
      wprintw(win, "Missing server ip address");
      exit(1);
    }

    serverAddr = argv[1]; //to get server ip address from command line

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        wprintw(win, "\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serverAddr);
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, serverAddr, &serv_addr.sin_addr)<=0)
    {
        wprintw(win, "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        wprintw(win, "\nConnection Failed \n");
        return -1;
    }

    //creating a new thread for receiving messages from the server
    int ret = pthread_create(&rThread, NULL, receiveMessage, (void *) sock);
    if (ret) {
        wprintw(win, "ERROR: Return Code from pthread_create() is %d\n", ret);
        exit(1);
    }    

    //Time stuff
    time_t last_time;
    float time_out = 0.3; //Time (seconds) before buffer changes are checked
    time(&last_time);
    timeout(-1);
    
    while(1) {
        ch = getch();
        //Quit the program, disconnect from host
        if(ch == CTRL('c')) {
            wprintw(win, "Bye bye\n");
            wrefresh(win);
            break;
        }
        // when user types in backspace
        else if (ch == 127 || ch == '\b' || ch == KEY_BACKSPACE) {
            char deleted_char = buffer[b_pos];
            b_pos = ((b_pos > 0) ? (b_pos-1) : 0);
            buffer[b_pos] = '\0';
            wprintw(win, "\b \b");
            wrefresh(win);
        }
        //Run the entered command
        else if(ch == '\n'){
            /*run commands*/
            //Send to server
            wprintw(win, "\n");
            ret = send(sock , buffer , BUF_SIZE , 0);
            if(ret < 0) {
                wprintw(win, "Error sending data");
                exit(1);
            }

        }
        //Sync between clients
        else {
            buffer[b_pos] = ch;
            b_pos++;
            waddch(win, ch | A_BOLD | A_UNDERLINE);
            //If update time has passed
            if (difftime(time(NULL), last_time) > time_out){
                //If buffer has updates
                if (strcmp(buffer, prev_buffer) == 0){
                    strcpy(prev_buffer, buffer); 
                    wprintw(win, "Sending Buffer %s\n", buffer);
                    wrefresh(win);
                    ret = send(sock , buffer , BUF_SIZE , 0);
                    if(ret < 0) {
                        wprintw(win, "Error sending data");
                        exit(1);
                    }
                }
                time(&last_time);
            }
        }
        wrefresh(win);
    } 
    
    getch();
    endwin();
    close(sock);
    pthread_exit(NULL);
    return 0;
}
