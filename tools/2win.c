#include <curses.h> 
 #include <menu.h> 
 #include <stdlib.h> 
 #include <ctype.h> 
#include "stdio.h"
 int main(int argc, char *argv[]) 
 { 
         int i; 
         int ch; 
         int lines, cols; 
         WINDOW *win, *win2,*winarea, *subwin; 
         ITEM *items[32]; 
         MENU *mymenu; 

         time_t tm; 
         tm = time(NULL); 

         if(initscr() == NULL) { 
                 perror("initcurs"); 
                 exit(EXIT_FAILURE); 
         } 
         cbreak(); 
         noecho(); 
         curs_set(1); 

         keypad(stdscr, TRUE); 
         getmaxyx(stdscr, lines, cols); 
         win = newwin(15, cols, 0, 0); 
         winarea = newwin(14, cols, 16, 0); 
         // win2 = newwin(lines/2, cols, 22, 0); 
         wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); 
         wrefresh(win); 
         box(win, 0, 0); 
         box(winarea, 0, 0); 

         wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); 
         wrefresh(win); 

         wborder(winarea, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); 
         wrefresh(winarea); 

         // scrollok(win, TRUE); 
         scrollok(winarea, TRUE); 
         wsetscrreg(win, 0, 15); 
         wsetscrreg(winarea, 0, 16); 

         mvwprintw(win, 0, 16, "Press Ctrl-C or 'Q' to exit; "); 
         mvwprintw(win, 2, 16, "Date: %s", ctime(&tm)); 
         mvwprintw(win, 2, 16, "Date: %s", ctime(&tm)); 

         wrefresh(win); 
         // wrefresh(winarea); 

         int k = 0; 
         // for(k = 0; k < 100; k ++){ 
         //         wprintw(win, "test_%d\n",k); 
         // }
         mvwprintw(winarea, 2, 0, "-------------------------------------------------"); 
         while(toupper(ch = wgetch(win))) { 
         	if (ch == 'i') {

         		// mvwprintw(win, 0, 44, "input i "); 
         		// scrl(2);
         		wscrl(win,2);
         		continue;
         	}
         	else if(ch == 'k') {
				// mvwprintw(win, 0, 44, "input d "); 
				wscrl(win,-2);
				continue;
         	}
         	wprintw(win, "%c",ch); 
         } 

         endwin(); 
         return 0; 
 } 