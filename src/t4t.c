/*
Written by Team Terms4Teams (Benjamin Ziemann, Seungin Lyu, Nathan Shuster)
Software Systems SP19 Olin College of Engineering
Credits to Stephen Brennan https://github.com/brenns10/lsh.
We used code from Stephen's simple lsh shell on top of ncurses library
and added collaboration fetaure by estabilishing network connections. 
*/


#include <ncurses.h>
#include <string.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 1024
#define TOKSIZE 64
#define TOKEN_DELIM " \t\r\n\a"

// Defining ctrl+c to get around ncurses raw() mode
#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif

/*
* Breaks the built up buffer into tokens. Called before running
* the line
*
* line - built up string buffer from user inputs
*/
char** parse_line(char* line){
    int bufsize = BUFSIZE;
    int b_position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;

    if (!tokens) {
        printw("Error: memory allocation failed for tokens\n");
        exit(1);
    }
    token = strtok(line, TOKEN_DELIM);
    while (token != NULL) {
        tokens[b_position] = token;
        b_position++;

        if (b_position >= bufsize) {
        bufsize += TOKSIZE;
        tokens_backup = tokens;
        tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                printw("Error: memory reallocation failed for tokens\n");
                exit(1);
            }
        }
        token = strtok(NULL, TOKEN_DELIM);
    }
    tokens[b_position] = NULL;
    return tokens;
}

/*
* Begins another process to execute the input line
*
* args - tokens of the input line arguements
*/
int run_line(char **args){
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("Fork error");
        }
        exit(1);
    } 
    else if (pid < 0) {
        // Error forking
        perror("Fork error");
        return 1;
    } 
    else {
        // Parent process
        do { 
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 0;
}


int main(){
    int ch; // Stored character for analysis
    int b_pos = 0; // position in the buffer
    int c_pos = 0; // ncurses cursor position
    char buffer[BUFSIZE];
    char **tokens;
    int status = 0; // status flag foor the terminal loop
    
    //Init ncurses
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    do {
        // sync()
            //Sync b_postion, buffer       
        ch = getch();
        printw("Got the char %c\n", ch);
        
        //Quit the program, disconnect from host
        if(ch == CTRL('c')) {
            printw("Bye bye\n");
            break;
        }
        
        // when user types in backspace
        else if (ch == 127 || ch == '\b' || ch == KEY_BACKSPACE) {
            char deleted_char = buffer[b_pos];
            b_pos = ((b_pos > 0) ? (b_pos-1) : 0);
            buffer[b_pos] = '\0';
            printw("new line: %s\n", buffer);
            printf("deleted char: %c\n", deleted_char);
        }
        
        //Run the entered command
        else if(ch == '\n'){
            buffer[b_pos] = '\0';
            char *line = strndup(buffer,b_pos);
            b_pos = 0;
            printw("Running line: %s\n", line);
            
            //Parse line
                //Break into tokens/arguments, identify primary command
            tokens = parse_line(buffer);
            //Run Line
            status = run_line(tokens);
        }
        //Sync between clients
        else {
            buffer[b_pos] = ch;
            b_pos++;
        }

        //Update ncurses
        refresh();
    } while(!status);
    
    getch();
    endwin();
    return 0;
}
