// Curses GUI utilities
// Author: Fernando Quintero
// Date: June 23,2006

#include "ucurses.h"

#define A_ATTR  (A_ATTRIBUTES ^ A_COLOR)	/* A_BLINK, A_REVERSE, A_BOLD */
#define TITLECOLOR         1	/* color pair indices */
#define MAINMENUCOLOR      (2 | A_BOLD)
#define MAINMENUREVCOLOR   (3 | A_BOLD | A_REVERSE)
#define SUBMENUCOLOR       (4 | A_BOLD)
#define SUBMENUREVCOLOR    (5 | A_BOLD | A_REVERSE)
#define BODYCOLOR          6
#define STATUSCOLOR        (7 | A_BOLD)
#define INPUTBOXCOLOR      8
#define EDITBOXCOLOR       (9 | A_BOLD | A_REVERSE)
#define BLACKONGREEN 1
#define WHITEONBLUE 2
#define WHITEONBLACK 3
#define BLACKONCYAN 4
#define REDONWHITE 5


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD   4

int menuCount = 0;
std::vector < std::vector < char const *> >myOpts;
int sem_id;
int msgid;


#ifndef _SEMUN_H
#define _SEMUN_H
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};
# endif

using namespace std;


// ------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------
CursesGui::CursesGui()
{

try {
    initscr();

    if (has_colors())
	start_color();
    init_pair(TITLECOLOR & ~A_ATTR, COLOR_BLACK, COLOR_CYAN);
    init_pair(MAINMENUCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_CYAN);
    init_pair(MAINMENUREVCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
    init_pair(SUBMENUCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_CYAN);
    init_pair(SUBMENUREVCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
    init_pair(BODYCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLUE);
    init_pair(STATUSCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_CYAN);
    init_pair(INPUTBOXCOLOR & ~A_ATTR, COLOR_BLACK, COLOR_CYAN);
    init_pair(EDITBOXCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_BLACK, COLOR_GREEN);	// for bar
    init_pair(2, COLOR_WHITE, COLOR_BLUE);	// for info and SUMMARY
    init_pair(3, COLOR_BLACK, COLOR_WHITE);	// for RECEIPT area
    init_pair(4, COLOR_BLACK, COLOR_CYAN);	// for entry area

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    assume_default_colors(COLOR_WHITE, COLOR_BLUE);
    } 
    catch( exception const &e)
    {
      cerr << "Error: " << e.what() << endl;
	} 

}

// ------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------

CursesGui::~CursesGui()
{
    // Reset everything to black
    assume_default_colors(COLOR_WHITE, COLOR_BLACK);
    clear();
    refresh();
    endwin();
}



/***********************************************************************/
/* Routine: printAt(x,y,text)                                          */
/* Purpose: To print a message in a specific position in the screen    */
/*                                                                     */
/***********************************************************************/


void CursesGui::printAt(int x, int y, std::string text)
{
    move(x, y);
    printw("%s", text.c_str());
    refresh();
}


/***********************************************************************/
/* Routine: printAt(x,x,int)                                           */
/* Purpose: To print a number in an specific position in the screen    */
/*                                                                     */
/***********************************************************************/


void CursesGui::printAt(int x, int y, int nInt)
{
    stringstream ss;
    ss << nInt;
    printAt(x, y, ss.str().c_str());
}


/***********************************************************************/
/* Routine: printAtWindow(WINDOW,x,y,text)                             */
/* Purpose: To write an specific text in a window                      */
/*                                                                     */
/***********************************************************************/
void CursesGui::printAtWindow(WINDOW * myWindow, int x, int y, string text)
{
    mvwprintw(myWindow, y, x, "%s", text.c_str());
    touchwin(myWindow);
    wrefresh(myWindow);
}


/***********************************************************************/
/* Routine: newWindow(x,y,numlines,numcols,chary,charx)                */
/* Purpose: To create a window in the screen                           */
/*                                                                     */
/***********************************************************************/


WINDOW *CursesGui::newWindow(int startx, int starty, int numLines,
			     int numCols, char boxChary, char boxCharx)
{
    WINDOW *myWin;
    myWin = newwin(numLines, numCols, startx, starty);
    box(myWin, boxChary, boxCharx);
    refresh();
    wrefresh(myWin);
    return myWin;
}


/***********************************************************************/
/*                                                                     */
/* Routine: newWindow(x,y,numlines,numcols,chary,charx)                */
/* Purpose: To create a window in the screen                           */
/*                                                                     */
/***********************************************************************/

WINDOW *CursesGui::newWindow(int startx, int starty, int numLines,
			     int numCols)
{
    WINDOW *myWin;
    myWin = newwin(numLines, numCols, startx, starty);
    refresh();
    wrefresh(myWin);
    return myWin;
}


/***********************************************************************/
/* Routine: moveWindow(WINDOW,x,y)                                     */
/* Purpose: To move a window to a specific position                    */
/*                                                                     */
/***********************************************************************/

void CursesGui::moveWindow(WINDOW * myWin, int x, int y)
{
    mvwin(myWin, y, x);
    wrefresh(myWin);
}


/***********************************************************************/
/* Routine: delWindow(WINDOW)                                          */
/* Purpose: To delete a window.                                        */
/*                                                                     */
/***********************************************************************/

void CursesGui::delWindow(WINDOW * myWin)
{
    delwin(myWin);
}


/***********************************************************************/
/* Routine: newMenu(options,numchoices)                                */
/* Purpose: To create a new menu.                                      */
/*                                                                     */
/***********************************************************************/

int CursesGui::newMenu(char const *options[], int n_choices)
{
    // add menu to the list
    int i;
    vector < char const*>myVector;

    for (i = 0; i < n_choices; ++i) {
	myVector.push_back(options[i]);
    }
    myOpts.push_back(myVector);
    menuCount++;
    return myOpts.size() - 1;
}

/***********************************************************************/
/* Routine: showMenu(menu)                                             */
/* Purpose: To show the menu created with newMenu in the screen.       */
/*                                                                     */
/***********************************************************************/

int CursesGui::showMenu(int menu)
{

    ITEM **my_items;
    int c;
    MENU *my_menu;
    int n_choices, i, quit;
    //ITEM *cur_item;
    //char* choices;
    stringstream ss;
    vector < const char *>count;
    WINDOW *my_menu_win, *my_sub_win;
    int option;

    quit = 0;
    option = 0;
    vector < char const *>myVector;

    keypad(stdscr, TRUE);
    myVector = myOpts[menu];
    n_choices = myVector.size();

    my_items = (ITEM **) calloc(n_choices + 1, sizeof(ITEM *));


    for (i = 0; i < n_choices; ++i) {
	ss << i;
	count.push_back(ss.str().c_str());
	my_items[i] = new_item(myVector[i], "");
    }

    my_items[n_choices] = (ITEM *) NULL;

    my_menu = new_menu((ITEM **) my_items);

    /* Create the window to be associated with the menu */
    int winsizey = (n_choices + 5);
    int winsizex = strlen(myVector[0]) + 10;
    my_menu_win =
	newwin(winsizey, winsizex, (LINES / 3) - 4, (COLS - winsizex) / 2);
    wborder(my_menu_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(my_menu_win);
    keypad(my_menu_win, TRUE);

    /* Set main window and sub window */
    set_menu_win(my_menu, my_menu_win);
    my_sub_win = derwin(my_menu_win, winsizey - 2, winsizex - 5, 2, 2);
    set_menu_sub(my_menu, my_sub_win);

    menu_opts_off(my_menu, O_SHOWDESC);

    /* Set menu mark to the string " -> " */
    set_menu_mark(my_menu, "> ");

    /* Print a border around the main window and print a title */

    /* MATCH MENU'S COLORS TO THAT OF ITS WINDOWS */
    set_menu_fore(my_menu, COLOR_PAIR(BLACKONCYAN));
    set_menu_back(my_menu, COLOR_PAIR(BLACKONCYAN) | WA_BOLD);

    setcolor(my_menu_win, BLACKONCYAN);

    mvwprintw(my_menu_win, winsizey + 2, 1, "F3 = Exit");
    refresh();
    post_menu(my_menu);
    wrefresh(my_menu_win);

    while (((c = getch()) != 10)) {
	switch (c) {
	case KEY_DOWN:
	    menu_driver(my_menu, REQ_DOWN_ITEM);
	    break;
	case KEY_UP:
	    menu_driver(my_menu, REQ_UP_ITEM);
	    break;
	case KEY_F(3):
	    option = KEY_F(3);
	    quit = 1;
	    break;
	case KEY_BACKSPACE:
	    option = KEY_F(3);
	    quit = 1;
	    break;
	}
	wrefresh(my_menu_win);

	if (quit)
	    break;
	ITEM *cur;
	cur = current_item(my_menu);
	option = item_index(cur);
    }

    free_menu(my_menu);

    for (i = 0; i < n_choices; ++i)
	free_item(my_items[i]);

    wclear(my_menu_win);
    wclear(my_sub_win);

    wrefresh(my_menu_win);
    wrefresh(my_sub_win);

    delwin(my_menu_win);
    delwin(my_sub_win);

    refresh();

    return option;
}



/***********************************************************************/
/* Routine: print_in_midle(WINDOW,y,x,w,string,color)                  */
/* Purpose: To print a centered text in a window.                      */
/*                                                                     */
/***********************************************************************/

void CursesGui::print_in_middle(WINDOW * win, int starty, int startx,
				int width, char const *string, chtype color)
{
    int length, x, y;
    float temp;

    if (win == NULL)
	win = stdscr;
    getyx(win, y, x);
    if (startx != 0)
	x = startx;
    if (starty != 0)
	y = starty;
    if (width == 0)
	width = 80;

    length = strlen(string);
    temp = (width - length) / 2;
    x = startx + (int) temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}


/***********************************************************************/
/* Routine: print_centered(y,x,string,color)                           */
/* Purpose: To print a centered text in default screen.                */
/*                                                                     */
/***********************************************************************/

void CursesGui::print_centered(int starty, int startx,
			       char const *string, chtype color)
{

    int length, x, y, width;
    float temp;

    x = startx;
    y = starty;
    width = COLS;
    length = strlen(string);
    temp = (width - length) / 2;
    x = x + (int) temp;
    attron(A_REVERSE);
    mvprintw(y, x, "%s", string);
    attroff(A_REVERSE);
    refresh();

}

/***********************************************************************/
/* Routine: printBox()                                                 */
/* Purpose: To print a box in the main screen.                         */
/*                                                                     */
/***********************************************************************/

void CursesGui::printBox(void)
{
  try {

    chtype color = COLOR_PAIR(3);
    chtype attr = color & A_ATTR;	/* extract Bold, Reverse, Blink bits */
    attr &= ~A_REVERSE;		/* ignore reverse, use colors instead! */
    attrset(COLOR_PAIR(color & A_CHARTEXT) | attr);
    if (has_colors())
	bkgd(COLOR_PAIR(color & A_CHARTEXT) | (attr & ~A_REVERSE));
    else
	bkgd(color);
    erase();
    border('|', '|', '-', '-', '+', '+', '+', '+');
    refresh();
    }
	catch(exception const &e)
	{
	  cerr << "Error: " << e.what() << endl;
	}

}

/***********************************************************************/
/* Routine: printTitle(string)                                         */
/* Purpose: To print the title.                                        */
/*                                                                     */
/***********************************************************************/

void CursesGui::printTitle(string title)
{
    print_centered(0, 1, (char const*) title.c_str(), A_STANDOUT);
}


/***********************************************************************/
/* Routine: colorbox(WINDOW,color,char)                                */
/* Purpose: To create a colored box                                    */
/*                                                                     */
/***********************************************************************/


void CursesGui::colorbox(WINDOW * win, chtype color, int hasbox)
{

    chtype attr = color & A_ATTR;	/* extract Bold, Reverse, Blink bits */
    setcolor(win, color);
    if (has_colors())
	wbkgd(win, COLOR_PAIR(color & A_CHARTEXT) | (attr & ~A_REVERSE));
    else
	wbkgd(win, color);
    werase(win);
    if (hasbox && win->_maxy > 2)
	box(win, 0, 0);
    touchwin(win);
    wrefresh(win);
}

/***********************************************************************/
/* Routine: setcolor(WINDOW,color)                                     */
/* Purpose: To change a window of an specified color.                  */
/*                                                                     */
/***********************************************************************/

void CursesGui::setcolor(WINDOW * win, chtype color)
{
    chtype attr = color & A_ATTR;	/* extract Bold, Reverse, Blink bits   */
    attr &= ~A_REVERSE;		/* ignore reverse, use colors instead! */
    wattrset(win, COLOR_PAIR(color & A_CHARTEXT) | attr);
    wattrset(win, COLOR_PAIR(color) | WA_BOLD);
    wclrscr(win);
    box(win, 0, 0);
    wCenterTitle(win, "");
}

/***********************************************************************/
/* Routine: messageBox(string)                                         */
/* Purpose: To send a message box to a screen.                         */
/*                                                                     */
/***********************************************************************/

void CursesGui::messageBox(string texto)
{
    ITEM **my_items;
    int c;
    MENU *my_menu;
    WINDOW *my_menu_win, *my_sub_win;
    char const *message = "  OK  ";


    keypad(stdscr, TRUE);

    my_items = (ITEM **) calloc(2, sizeof(ITEM *));
    my_items[0] = new_item(message, "");

    my_items[1] = (ITEM *) NULL;
    my_menu = new_menu((ITEM **) my_items);
    menu_opts_off(my_menu, O_SHOWDESC);

    int winsizey = 6;
    int winsizex = strlen(texto.c_str()) + 10;

    my_menu_win =
	newwin(winsizey, winsizex, (LINES / 3) - 4, (COLS - winsizex) / 2);
    wborder(my_menu_win, '|', '|', '-', '-', '+', '+', '+', '+');
    keypad(my_menu_win, TRUE);

    /* Set main window and sub window */
    set_menu_win(my_menu, my_menu_win);
    my_sub_win = derwin(my_menu_win, 2, 8, 4, (strlen(texto.c_str())) / 2);
    set_menu_sub(my_menu, my_sub_win);
    menu_opts_off(my_menu, O_SHOWDESC);
    set_menu_mark(my_menu, 0);


    /* Print a border around the main window and print a title */
    set_menu_fore(my_menu, COLOR_PAIR(BLACKONCYAN) | WA_REVERSE);
    set_menu_back(my_menu, COLOR_PAIR(BLACKONCYAN) | WA_BOLD);
    setcolor(my_menu_win, BLACKONCYAN);

    print_in_middle(my_menu_win, 1, 1, strlen(texto.c_str()) + 8,
		    (char const*) texto.c_str(),
		    COLOR_PAIR(BLACKONCYAN) | WA_BOLD);
    post_menu(my_menu);
    wrefresh(my_menu_win);

    while ((c = getch()) != 10) {
	switch (c) {
	case KEY_DOWN:
	    menu_driver(my_menu, REQ_DOWN_ITEM);
	    break;
	case KEY_UP:
	    menu_driver(my_menu, REQ_UP_ITEM);
	    break;
	}
	wrefresh(my_menu_win);
    }

    free_menu(my_menu);

    free_item(my_items[0]);
    free_item(my_items[1]);

    wclear(my_menu_win);
    wclear(my_sub_win);

    wrefresh(my_menu_win);
    wrefresh(my_sub_win);

    delwin(my_menu_win);
    delwin(my_sub_win);

    return;

}


/***********************************************************************/
/*  Routine: yesno(string)                                             */
/*  Purpose: To send a yes/no message                                  */
/*                                                                     */
/***********************************************************************/

int CursesGui::yesno(string texto)
{
    ITEM **my_items;
    int c;
    MENU *my_menu;
    WINDOW *my_menu_win, *my_sub_win;
    char const *yes = "  YES  ";
    char const *no = "   NO  ";


    keypad(stdscr, TRUE);

    my_items = (ITEM **) calloc(3, sizeof(ITEM *));
    my_items[0] = new_item(yes, "");
    my_items[1] = new_item(no, "");

    my_items[2] = (ITEM *) NULL;

    my_menu = new_menu((ITEM **) my_items);

    int winsizey = 6;
    int winsizex = strlen(texto.c_str()) + 10;

    my_menu_win =
	newwin(winsizey, winsizex, (LINES / 3) - 4, (COLS - winsizex) / 2);
    wborder(my_menu_win, '|', '|', '-', '-', '+', '+', '+', '+');
    keypad(my_menu_win, TRUE);

    /* Set main window and sub window */
    set_menu_win(my_menu, my_menu_win);

    int colSize = strlen(texto.c_str());
    if (colSize < 20) {
	colSize = 20;
    }

    my_sub_win =
	derwin(my_menu_win, 2, colSize, winsizey - 2,
	       (winsizex - colSize));

    set_menu_sub(my_menu, my_sub_win);
    set_menu_format(my_menu, 1, 2);
    menu_opts_off(my_menu, O_SHOWDESC);
    set_menu_mark(my_menu, 0);

    /* Print a border around the main window and print a title */
    set_menu_fore(my_menu, COLOR_PAIR(BLACKONCYAN) | WA_REVERSE);
    set_menu_back(my_menu, COLOR_PAIR(BLACKONCYAN));
    setcolor(my_menu_win, BLACKONCYAN);

    print_in_middle(my_menu_win, 1, 1, colSize + 2,
		    (char const*) texto.c_str(), COLOR_PAIR(BLACKONCYAN));
    post_menu(my_menu);
    wrefresh(my_menu_win);

    while ((c = getch()) != 10) {
	switch (c) {
	case KEY_DOWN:
	    menu_driver(my_menu, REQ_DOWN_ITEM);
	    break;
	case KEY_UP:
	    menu_driver(my_menu, REQ_UP_ITEM);
	    break;
	case KEY_LEFT:
	    menu_driver(my_menu, REQ_LEFT_ITEM);
	    break;
	case KEY_RIGHT:
	    menu_driver(my_menu, REQ_RIGHT_ITEM);
	    break;
	}
	wrefresh(my_menu_win);
    }
    ITEM *cur;
    cur = current_item(my_menu);
    int option = item_index(cur);

    free_menu(my_menu);

    free_item(my_items[0]);
    free_item(my_items[1]);
    free_item(my_items[2]);

    wclear(my_menu_win);
    wclear(my_sub_win);

    wrefresh(my_menu_win);
    wrefresh(my_sub_win);

    delwin(my_menu_win);
    delwin(my_sub_win);

    return option;

}

void CursesGui::wait_for_key(void)
{
 try {

  char c;
    cin.get(c);
    while ( c  != 10) {
         cin.get(c);
    }
  } 
  catch(exception const &e)
  {
	cerr << " Error: " << e.what()  << endl;
  }

}


void CursesGui::helloworld(void)
{
	cout << "I'm alive"<<endl;
}


/***********************************************************************/
/* Routine: dialogBox(string,size)                                     */
/* Purpose: To send a dialog box.                                      */
/*                                                                     */
/***********************************************************************/

string CursesGui::dialogBox(string texto, int nsize)
{
    FIELD *field[2];
    FORM *my_form;
    int ch;
    WINDOW *my_form_win, *my_sub_win;

    keypad(stdscr, TRUE);

    int winlines = 6;
    int wincols = nsize + 4;
    int colSize = strlen(texto.c_str());
    if (wincols < colSize)
	wincols = colSize + 4;
    int subwincols = nsize + 1;
    int subwinlines = winlines - 4;
    int subwinposx = 2;
    int subwinposy = 3;

    /* Initialize the fields */
    field[0] = new_field(1, nsize, 0, 0, 0, 0);
    field[1] = NULL;

    /* Set field options */


    /* Create the form and post it */
    my_form = new_form(field);
    set_current_field(my_form, field[0]);	/* Set focus to the colored field */
    field_opts_off(field[0], O_WRAP);
    field_opts_off(field[0], O_AUTOSKIP);

    my_form_win =
	newwin(winlines, wincols, (LINES / 3) - 4, (COLS - wincols) / 2);
    set_form_win(my_form, my_form_win);

    my_sub_win = derwin(my_form_win, subwinlines, subwincols, subwinposy,
			subwinposx);

    setcolor(my_sub_win, BLACKONCYAN);
    set_form_sub(my_form, my_sub_win);

    keypad(my_form_win, TRUE);

    /* Print a border around the main window and print a title */

    setcolor(my_form_win, BLACKONCYAN);

    wbkgd(my_sub_win, COLOR_PAIR(BLACKONCYAN & A_CHARTEXT));

    set_field_fore(field[0],
		   COLOR_PAIR(BLACKONCYAN) | A_UNDERLINE | A_REVERSE);
    set_field_back(field[0],
		   COLOR_PAIR(BLACKONCYAN) | A_UNDERLINE | A_REVERSE);
    set_field_pad(field[0],
		  COLOR_PAIR(BLACKONCYAN) | A_UNDERLINE | A_REVERSE);

    print_in_middle(my_form_win, 1, 1, colSize + 2,
		    (char const*) texto.c_str(), COLOR_PAIR(BLACKONCYAN));

    post_form(my_form);
    wrefresh(my_form_win);


    /* Loop through to get user requests */
    while ((ch = wgetch(my_form_win)) != 10) {
	switch (ch) {
	case KEY_BACKSPACE:
	    form_driver(my_form, REQ_DEL_PREV);
	    break;
	case KEY_DC:
	    form_driver(my_form, REQ_DEL_PREV);
	    break;
	case KEY_LEFT:
	    form_driver(my_form, REQ_PREV_CHAR);
	    break;
	case KEY_RIGHT:
	    form_driver(my_form, REQ_NEXT_CHAR);
	    break;
	default:
	    /* If this is a normal character, it gets */
	    /* Printed                                */
	    form_driver(my_form, ch);
	    break;
	}

    }

    form_driver(my_form, REQ_END_LINE);
    string response(field_buffer(field[0], 0));

    refresh();
    wclear(my_form_win);
    wclear(my_sub_win);

    wrefresh(my_form_win);
    wrefresh(my_sub_win);


    /* Un post form and free the memory */
    unpost_form(my_form);
    free_form(my_form);
    free_field(field[0]);
    free_field(field[1]);


    return response.c_str();

}


/***********************************************************************/
/* Routine: wclrscr(WINDOW)                                            */
/* Purpose: to clear a window                                          */
/*                                                                     */
/***********************************************************************/
void CursesGui::wclrscr(WINDOW * pwin)
{
    int y, x, maxy, maxx;
    getmaxyx(pwin, maxy, maxx);
    for (y = 0; y < maxy; y++)
	for (x = 0; x < maxx; x++)
	    mvwaddch(pwin, y, x, ' ');
}

/***********************************************************************/
/* Routine: wCenterTitle(WINDOW,title)                                 */
/* Purpose: To write a title in a window                               */
/*                                                                     */
/***********************************************************************/
void CursesGui::wCenterTitle(WINDOW * pwin, const char *title)
{
    int x, maxy, maxx, stringsize;
    getmaxyx(pwin, maxy, maxx);
    stringsize = 4 + strlen(title);
    x = (maxx - stringsize) / 2;
    mvwaddch(pwin, 0, x, ACS_RTEE);
    waddch(pwin, ' ');
    waddstr(pwin, title);
    waddch(pwin, ' ');
    waddch(pwin, ACS_LTEE);
}

/***********************************************************************/
/* Routine: fileView(fname)                                            */
/* Purpose: To show a file in a screen                                 */
/*                                                                     */
/***********************************************************************/

int CursesGui::fileView(string fname)
{
    int cnt, option;

    ifstream myfile;
    string data, line;

    cnt = countLines(fname.c_str());
    myfile.open(fname.c_str());

    if (myfile.is_open()) {

	while (!myfile.eof()) {
	    getline(myfile, line);
	    data.append(line);
	    data.append("\n");
	}			/* End while */
    }
    myfile.close();
    option = view(data, cnt);
    return option;
}

/***********************************************************************/
/* Routine: fileViewIPC(fname)                                         */
/* Purpose: To show a file in a window, but waiting for a message in   */
/*          a message queue.                                           */
/***********************************************************************/

int CursesGui::fileViewIPC(string fname)
{
    int cnt, option;

    ifstream myfile;
    string data, line;

    cnt = countLines(fname.c_str());
    myfile.open(fname.c_str());

    if (myfile.is_open()) {

	while (!myfile.eof()) {
	    getline(myfile, line);
	    data.append(line);
	    data.append("\n");
	}			/* End while */
    }
    myfile.close();
    option = viewN(data, cnt);
    return option;
}




/***********************************************************************/
/* Routine: execView(cmd)                                              */
/* Purpose: To execute a command an show the output in a window        */
/*                                                                     */
/***********************************************************************/

int CursesGui::execView(string cmd)
{

    FILE *read_fp;
    char buffer[BUFSIZ + 1];
    int lines;
    string cmdOut;


    memset(buffer, '\0', sizeof(buffer));
    read_fp = popen(cmd.c_str(), "r");
    if (read_fp != NULL) {

	while (fgets(buffer, sizeof(buffer), read_fp)) {
	    cmdOut.append(buffer);
	    memset(buffer, '\0', sizeof(buffer));
	}

	lines = countChars(cmdOut);
	view(cmdOut, lines);
	pclose(read_fp);
	return (EXIT_SUCCESS);
    }
    return (EXIT_FAILURE);
}


/***********************************************************************/
/* Routine: view(string,lines)                                         */
/* Purpose: To show a string data in a window                          */
/*                                                                     */
/***********************************************************************/

int CursesGui::view(string data, int lines)
{
    int ch;
    WINDOW *my_form_win, *my_pad_win;
    string line;

    keypad(stdscr, TRUE);

    int winlines = LINES - 4;
    int wincols = COLS - 2;
    int subwincols = COLS - 5;
    int subwinlines = LINES - 7;

    my_pad_win = newpad(lines * 3, wincols - 5);
    my_form_win = newwin(winlines, wincols, 2, 1);
    keypad(my_form_win, TRUE);

    int index, cnt;
    unsigned int x;
    string mysubstr;
    index = 0;
    x = 0;
    cnt = 0;

    while (x < data.size()) {
	x = data.find("\n", index);
	mysubstr = data.substr(index, (x - index));
	index = x;
	index++;
	mvwprintw(my_pad_win, cnt, 0, " %s", mysubstr.c_str());
	int sum = strlen(mysubstr.c_str()) / wincols + 1;
	cnt = cnt + sum;
	mysubstr.clear();
    }

    wborder(my_form_win, '|', '|', '-', '-', '+', '+', '+', '+');
    int first_line = 0;
    int first_col = 0;
    wrefresh(my_form_win);
    prefresh(my_pad_win, first_line, first_col, 3, 2, subwinlines,
	     subwincols);


    /* Loop through to get user requests */
    ch = wgetch(my_form_win);

    while (ch != KEY_F(3) && ch != KEY_BACKSPACE) {

	switch (ch) {
	case KEY_UP:
	    if (first_line > 0)
		first_line--;
	    break;
	case KEY_DOWN:
	    if (first_line <= (lines - (winlines - 4)))
		first_line++;
	    break;
	    break;

	}
	prefresh(my_pad_win, first_line, first_col, 3, 2, subwinlines,
		 subwincols);
	wrefresh(my_form_win);
	ch = wgetch(my_form_win);
    }


    refresh();
    wclear(my_form_win);
    wrefresh(my_form_win);

    return ch;
}



/***********************************************************************/
/* Routine: viewN(data,lines)                                          */
/* Purpose: To show a window with data and wait for a message in a     */
/*          message queue.                                             */
/***********************************************************************/

int CursesGui::viewN(string data, int lines)
{
    int ch;
    WINDOW *my_form_win, *my_pad_win;
    string line;



    keypad(stdscr, TRUE);


    int winlines = LINES - 4;
    int wincols = COLS - 2;
    int subwincols = COLS - 5;
    int subwinlines = LINES - 7;

    my_pad_win = newpad(lines * 3, wincols - 5);
    my_form_win = newwin(winlines, wincols, 2, 1);
    keypad(my_form_win, TRUE);

    int index, cnt;
    unsigned int x;
    string mysubstr;
    index = 0;
    x = 0;
    cnt = 0;

    while (x < data.size()) {
	x = data.find("\n", index);
	mysubstr = data.substr(index, (x - index));
	index = x;
	index++;
	mvwprintw(my_pad_win, cnt, 0, " %s", mysubstr.c_str());
	int sum = strlen(mysubstr.c_str()) / wincols + 1;
	cnt = cnt + sum;
	mysubstr.clear();
    }

    wborder(my_form_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wCenterTitle(my_form_win, "CTRL-C to Exit");
    int first_line = 0;
    int first_col = 0;
    wrefresh(my_form_win);
    prefresh(my_pad_win, first_line, first_col, 3, 2, subwinlines,
	     subwincols);

    // Wait for CTRL-C
    ch = msgGet();
    wclear(my_form_win);
    wrefresh(my_form_win);
    refresh();
    return ch;
}

/***********************************************************************/
/* Routine: countLines(fname)                                          */
/* Purpose: To count the lines in a filename.                          */
/*                                                                     */
/***********************************************************************/

int CursesGui::countLines(string filename)
{


    ifstream myfile;
    string line;
    int cnt=0;

    myfile.open(filename.c_str());

    if (myfile.is_open()) {
	cnt = 0;
	while (!myfile.eof()) {
	    getline(myfile, line);
	    cnt++;
	}			/* End while */
    }
    myfile.close();
    return cnt;

}

/***********************************************************************/
/*  Routine: countChars(string)                                        */
/*  Purpose: To find the number of end of lines in a string there are  */
/*                                                                     */
/***********************************************************************/

int CursesGui::countChars(string mychar)
{
    unsigned int cnt, x, index;

    cnt = 0;
    index = 0;
    x = 0;
    while (x < mychar.size()) {
	x = mychar.find("\n", index);
	index = x;
	cnt++;
	index++;
    }
    return cnt;
}


/***********************************************************************/
/* Routine: msgGet(void)                                               */
/* Purpose: To get a message from a message queue.                     */
/*                                                                     */
/***********************************************************************/



int CursesGui::msgGet()
{

    struct msg_st {
	long int msg_type;
	int msg[BUFSIZ];
    };

    struct msg_st data;
    long int msg_to_receive = 0;

    msgid = msgget((key_t) 42114, 0666 | IPC_CREAT);
    if (msgid == -1) {
	//fprintf(stderr,"msgget failed\n");
	return -1;
    }
    if (msgrcv(msgid, (void *) &data, BUFSIZ, msg_to_receive, 0) == -1) {
	//fprintf(stderr,"msgrcv failed\n");
	return -1;
    }
    if (msgctl(msgid, IPC_RMID, 0) == -1) {
	fprintf(stderr, "msgctl(IPC_RMID) failed\n");
	return -1;
    }
    return data.msg[0];
}


int CursesGui::getLines()
{
  return LINES;
}
