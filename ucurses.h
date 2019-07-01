#ifndef U_CURSES_H
#define U_CURSES_H

// Curses GUI utilities
// Author: Fernando Quintero
// Date: June 23,2006

#include <stdlib.h>
#include <string.h>
#include "ncurses.h"
#include "form.h"
#include <menu.h>
#include <panel.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <cstring> 
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

class CursesGui {
  public:
    void helloworld(void);
    int getLines();
    int newMenu(char const *[], int);
    int showMenu(int);
    void wait_for_key(void);
    void printBox(void);
    void printTitle(std::string title);
    void printAt(int, int, std::string);
    void printAt(int, int, int);
    void printAtWindow(WINDOW *, int, int, std::string);
    void print_in_middle(WINDOW * win, int starty, int startx, int width,
			 char const *string, chtype color);
    void colorbox(WINDOW * win, chtype color, int hasbox);
    void setcolor(WINDOW * win, chtype color);
    void messageBox(std::string);
    int yesno(std::string texto);
    void print_centered(int starty, int startx, char const *string,
			chtype color);
    WINDOW *newWindow(int, int, int, int);
    WINDOW *newWindow(int, int, int, int, char, char);
    void moveWindow(WINDOW *, int, int);
    void delWindow(WINDOW *);
    std::string dialogBox(std::string, int);
    void wclrscr(WINDOW *);
    void wCenterTitle(WINDOW *,  char const *);
    int fileView(std::string);
    int execView(std::string);
    int fileViewIPC(std::string fname);

    // constructor and destructor

     CursesGui();
    ~CursesGui();

  private:
    int countChars(std::string);
    int view(std::string, int);
    int countLines(std::string);
    int msgGet(void);
    int viewN(std::string data, int lines);


};

#endif
