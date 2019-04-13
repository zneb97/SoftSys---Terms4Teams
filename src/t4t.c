#include <ncurses.h>
#include <string.h> 

int main(){
    int ch;
    
    initscr();
    //ncurses Init methods
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    printw("Type any character to see it in bold\n");
    while(1){
        ch = getch();
        if(ch == EOF) break;
        else{
            printw("%c", ch);
        }
    }
    refresh();
    getch();
    endwin();
    return 0;
}
