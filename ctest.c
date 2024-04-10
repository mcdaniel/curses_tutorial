////////////////////////////////////////////////////////////////////////////////
//
//  File          : ctest.cpp
//  Description   : This is a set of test code for the curses library.
//
//   Author : Patrick McDaniel
//   Last Modified : Thu Jan 18 16:13:39 EST 2024
//

// Include Files
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <form.h>

//
// Defines
#define RGB_TO_CURSES(x) ((int)(round((float)x*3.90625)))  // 1000/256 conversion
#define CURSES_TO_RGB(x) ((int)(round((float)x/3.90625)))  // 256/1000 conversion

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : ncurses_basic
// Description  : Test some basic functions of the ncurses library
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int ncurses_basic( void ) {
    
        // Local variables
        int ch, i, j, x, y, sy, sx, pos, done;
        short badger_red = COLOR_WHITE + 1;
        MEVENT event;
        WINDOW *ann, *win;
    
        // Print out some information to the screen
        printw("CTest: testing the curses function\n");
        refresh();
    
        // Get the screen dimensions, fill the screen with X's
        getmaxyx(stdscr, y, x); // Gets the size of the window
        for (i=0; i<y; i++) {
            for (j=0; j<x; j++) {
                mvaddch(i, j, '.' | A_REVERSE | A_BOLD);
            }
        }
        mvprintw(1,0, "The screen size is %d rows by %d columns", y, x);
        refresh();
        getch();

        // Create a new window for showing annotations
        ann = newwin(12, 40, 22, 50);
        pos = 1;
        box(ann, 0, 0);
        wattrset(ann, A_NORMAL);
        mvwprintw(ann, pos++, 1, "This is normal text");
        wattrset(ann, A_STANDOUT);
        mvwprintw(ann, pos++, 1, "This is standout text");
        wattrset(ann, A_UNDERLINE);
        mvwprintw(ann, pos++, 1, "This is underline text");
        wattrset(ann, A_REVERSE);
        mvwprintw(ann, pos++, 1, "This is reverse text");
        wattrset(ann, A_BLINK);
        mvwprintw(ann, pos++, 1, "This is blink text");
        wattrset(ann, A_DIM);
        mvwprintw(ann, pos++, 1, "This is dim text");
        wattrset(ann, A_BOLD);
        mvwprintw(ann, pos++, 1, "This is bold text");
        wattrset(ann, A_PROTECT);
        mvwprintw(ann, pos++, 1, "This is protected text");
        wattrset(ann, A_INVIS);
        mvwprintw(ann, pos++, 1, "This is invisible text");
        wattrset(ann, A_ALTCHARSET);
        mvwprintw(ann, pos++, 1, "This is alternate charater set text");
        wrefresh(ann);
        getch();
        wclear(ann);
        wrefresh(ann);
        delwin(ann);
        refresh();
        doupdate();
        getch();

        // Create a new window
        win = newwin(20, 60, 35, 5);
        box(win, 0, 0);
        getmaxyx(win, sy, sx);
        mvprintw(2,0, "The window size is %d rows by %d columns", sy, sx);
        mvwvline(win, 1, 5, ACS_VLINE, 8);
        wrefresh(win);
        getch();

        // Check if the terminal supports color, and if so, use it
        if ( has_colors() == TRUE ) {

            // Start the color system, play with the color
            start_color();
            init_color(badger_red, RGB_TO_CURSES(197), RGB_TO_CURSES(5), RGB_TO_CURSES(12));
            init_pair(1, badger_red, COLOR_WHITE);
            wattr_on(win, COLOR_PAIR(1), NULL);
            mvwprintw(win, 3, 0, "This is now a special UW-Madison color!");

            // Get the color pair information
            short pindex = 1, foreground, background;
            pair_content(pindex, &foreground, &background);
            mvprintw(4, 1, "The color pair 1 has foreground %d and background %d", foreground, background);

            // Get the color information
            short red, green, blue;
            color_content(badger_red, &red, &green, &blue);
            mvprintw(5, 1, "The color pair %d has red %d, green %d, and blue %d", 
                    badger_red, CURSES_TO_RGB(red),  CURSES_TO_RGB(green), CURSES_TO_RGB(blue));
            wrefresh(win);

        } else {
            mvwprintw(win, 3, 0, "This terminal does not support color!");
        }

        // Wait for a key press
        mvwprintw(win, 6, 0, "Press the up arrow key.");
        ch = getch();
        if ( ch == KEY_UP ) {
            mvwprintw(win, 7, 0, "You pressed the up arrow key");
        } else {
            mvwprintw(win, 7, 0, "You pressed the key %c", ch);
        }
        wrefresh(win);
        getch();

        // Now do mouse stuf
        mmask_t mask, oldmask;
        mask = BUTTON1_PRESSED | BUTTON_SHIFT | BUTTON2_CLICKED | REPORT_MOUSE_POSITION;
        mousemask(mask, &oldmask); // Assigns new mask and saves previous one
        mouseinterval(0); // No delay for mouse clicks

        // Wait for a mouse click
        mvprintw(0, 0, "Click the left mouse button in and outside the window.");
        done = 0;
        while (!done) {
            ch = getch();
            if (ch == KEY_MOUSE) {
                if (getmouse(&event) == OK) {
                    bool inwindow = wenclose(win, event.y, event.x);
                    int wx = event.x, wy = event.y, sx, sy;
                    mvprintw(11, 1, "The event coordinates are %d, %d", event.y, event.x);
                    clrtoeol();
                    if (wmouse_trafo(win, &wy, &wx, FALSE) == TRUE) {  // Convert screen -> window coordinates
                        mvprintw(12, 1, "The window coordinates are %d, %d and the screen coordinates are %d, %d", wy, wx, event.y, event.x);
                        sx = wx; sy = wy;
                        wmouse_trafo(win, &sy, &sx, TRUE);  // Convert screen -> window coordinates
                        mvprintw(13, 1, "The window coordinates are %d, %d(converted back)", sy, sx);
                        clrtoeol();
                    } else {
                        mvprintw(12, 1, "The screen coordinates are %d, %d and not in the window.", wy, wx);
                        clrtoeol();
                    }
                    wrefresh(win);
                    refresh();
                    if (event.bstate & BUTTON1_PRESSED) {
                        mvwprintw(win, 8, 1, "You pressed the left mouse button at %d, %d [in window? %d]", event.x, event.y, inwindow);
                        if (inwindow  == TRUE) { 
                            wmouse_trafo(win, &event.y, &event.x, FALSE);
                            mvwaddch(win, event.y, event.x, ACS_DIAMOND | A_REVERSE | A_BOLD);
                            wmove(win, event.y, event.x);
                            wrefresh(win);

                        } else {
                            mvaddch(event.y, event.x, ACS_BLOCK | A_REVERSE | A_DIM);
                            move(event.y, event.x);
                            refresh();
                        }
                    }
                    if (event.bstate & BUTTON2_CLICKED) {
                        mvwprintw(win, 9, 1, "You clicked the right mouse button at %d, %d", event.x, event.y);
                        clrtoeol();
                        wrefresh(win);
                    }
                    if (event.bstate & REPORT_MOUSE_POSITION) {
                        mvwprintw(win, 10, 1, "You moved the mouse to %d, %d", event.x, event.y);
                        clrtoeol();
                        wrefresh(win);
                        mvaddch(event.y, event.x, 'X' | A_REVERSE | A_BOLD);
                        refresh();
                    }
                }

            }
            if (ch == 'q') {
                done = 1;
            }
        }

        // Clean up the window, clear the screen, and return
        delwin(win);
        erase();
        return( 0 );
    
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : panel_basic
// Description  : Test of the panel library
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int panel_basic( void ) {

    // Local variables
    PANEL *panels[10];
    WINDOW *win, *swin;
    int i;

    // Loop through some random panels
    for (i=0; i<10; i++) {
        win = newwin(10, 20, 5+i*2, 5+i*2);
        box(win, 0, 0);
        mvwprintw(win, 1, 1, "This is window %d", i);
        wrefresh(win);
        panels[i] = new_panel(win);
    }

    // Move through the panels
    for (i=0; i<10; i++) {
        top_panel(panels[i]);
        update_panels();
        doupdate();
        getch();
    }

    // Create a new window to demonstrate subwindows
    win = newwin(20, 60, 35, 5), 
    swin = derwin(win, 10, 20, 5, 5);
    box(win, 0, 0);
    box(swin, 0, 0);
    mvwprintw(win, 1, 1, "This is the main window");
    mvwprintw(swin, 1, 1, "Subwindow");
    wrefresh(win);
    wrefresh(swin);
    getch();
    werase(swin);
    wrefresh(swin);
    getch();
    werase(win);
    wrefresh(win);
    getch();
    refresh();

    // Clean up the panels, return
    for (int i=0; i<10; i++) {
        del_panel(panels[i]);
    }

    // Return successfully
    return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : menu_basic
// Description  : Test some basic functions of the menu library
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

#define BASIC_ELEMENTS 5
int menu_basic( int do_window ) {

    // Local variables
    int ch, done;
    ITEM *basic_items[BASIC_ELEMENTS + 1];
    MENU *basic_menu;
    WINDOW *win;

    // Create the basic items, menu
    basic_items[0] = new_item("Item 1", "Description 1");
    basic_items[1] = new_item("Item 2", "Description 2");
    basic_items[2] = new_item("Item 3", "Description 3");
    basic_items[3] = new_item("Item 4", "Description 4");
    basic_items[4] = new_item("Item 5", "Description 5");
    basic_items[5] = NULL;
    basic_menu = new_menu((ITEM **)basic_items);

    // Add some color to the menu
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    set_menu_fore(basic_menu, COLOR_PAIR(1) | A_REVERSE);
    set_menu_back(basic_menu, COLOR_PAIR(2));
    set_menu_grey(basic_menu, COLOR_PAIR(3));

    // If we are doing window, setup the window
    if ( do_window == 1 ) {
        // Window version
        win = newwin(20, 60, 5, 5);
        box(win, 0, 0);
        set_menu_win(basic_menu, win);
        set_menu_sub(basic_menu, derwin(win, 6, 38, 3, 1));
        wmove(win, 1, 1);
        keypad(win, TRUE);
        post_menu(basic_menu);
        wrefresh(win);
    } else {
        // Screen version
        keypad(stdscr, TRUE);
        post_menu(basic_menu);
        refresh();
    }

    // Get input for the menu
    done = 0;
    while ( !done ) {
        ch = do_window ? wgetch(win) : getch();
        switch (ch) {
            case KEY_DOWN:
            case 's': // s is up:
                mvprintw(9, 1, "You pressed the down arrow key");
                menu_driver(basic_menu, REQ_DOWN_ITEM);
                break;
            case 'w': // w is up
            case KEY_UP:
                mvprintw(9, 1, "You pressed the up arrow key");
                menu_driver(basic_menu, REQ_UP_ITEM);
                break;
            case 'q': // Quit
                done = 1;
                break;
        }

        // Do a refresh
        if ( do_window == 1 ) {
            wrefresh(win);
        } else {
            refresh();
        }
    }

    // Cleanup the menu
    unpost_menu(basic_menu);
    for (int i=0; i<BASIC_ELEMENTS; i++) {
        free_item(basic_items[i]);
    }
    free_menu(basic_menu);

    // Return successfully
    return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : form_basic
// Description  : A basic test of the form functions
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int form_basic( int do_window ) {

    // Local variables
    FORM *test_form;
    FIELD *test_fields[5];
    WINDOW *win = NULL;
    int ch, done = 0, rows, cols;

    // Create the fields, then form
    test_fields[0] = new_field(1, 5, 0, 0, 0, 0);
    test_fields[1] = new_field(1, 5, 1, 0, 0, 0);
    test_fields[2] = new_field(1, 10, 0, 6, 0, 0);
    test_fields[3] = new_field(1, 10, 1, 6, 0, 0);
    test_fields[4] = NULL;
    test_form = new_form(test_fields);

    // Setup the labels
    field_opts_off(test_fields[0], O_EDIT|O_ACTIVE);
    set_field_buffer(test_fields[0], 0, "FLD1:");
    field_opts_off(test_fields[1], O_EDIT|O_ACTIVE);
    set_field_buffer(test_fields[1], 0, "FLD2:");

    // Setup some colors
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);

    // Set the field options
    set_field_back(test_fields[0], COLOR_PAIR(3));
    set_field_back(test_fields[1], COLOR_PAIR(3));
    set_field_just(test_fields[2], JUSTIFY_CENTER);
    set_field_back(test_fields[2], COLOR_PAIR(1));
    set_field_fore(test_fields[2], COLOR_PAIR(2));
    set_field_back(test_fields[3], COLOR_PAIR(1));
    set_field_fore(test_fields[3], COLOR_PAIR(2));

    // If we are doing window, setup the window
    if ( do_window == 1 ) {
        // Window version
        win = newwin(20, 60, 3, 3);
        keypad(win, TRUE);
        wrefresh(win);
        scale_form(test_form, &rows, &cols);
        set_form_win(test_form, win);
        set_form_sub(test_form, derwin(win, rows, cols, 2, 2));
        post_form(test_form);
        wrefresh(win);
    } else {
        // Screen version
        keypad(stdscr, TRUE);
        post_form(test_form);
        refresh();
    }

    // Wait for a key press
    while ( !done ) {
        ch = getch();
        switch (ch) {
            case KEY_DOWN:
                mvprintw(9, 1, "You pressed the down arrow key");
                break;
            case KEY_UP:
                mvprintw(9, 1, "You pressed the up arrow key");
                form_driver(test_form, REQ_PREV_FIELD);
                break;
            case 'q': // Quit
                done = 1;
                break;
            default:
                form_driver(test_form, ch);
                break;
        }

        // Do a refresh
        if ( do_window == 1 ) {
            wrefresh(win);
        } else {
            refresh();
        }
    }

    // Cleanup the form
    unpost_form(test_form);
    free_form(test_form);
    free_field(test_fields[0]);
    free_field(test_fields[1]);
    free_field(test_fields[2]);
    free_field(test_fields[3]);

    // Cleanup the window
    if ( do_window == 1 ) {
        delwin(win);
    }

    // Return successfully
    return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : main
// Description  : The main function for the PW database manager
//
// Inputs       : argc - the number of command line parameters
//                argv - the parameters
// Outputs      : 0 if successful test, -1 if failure

int main(int argc, char *argv[]) {

    // Setup ncurses library
    initscr();
    noecho();
    cbreak();
    raw();
    keypad(stdscr, TRUE);
    curs_set(2);

    ncurses_basic();
    panel_basic();
    menu_basic(0);
    menu_basic(1);
    form_basic(0);
    form_basic(1);

    // End the curses library
    endwin();
    echo();
    curs_set(1);

    // Return successfully
    printf("CTest: successful.\n");
    return(0);
}