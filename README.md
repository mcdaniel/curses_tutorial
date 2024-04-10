# Curses (ncurses) tutorial

This repo contains a very brief and to the point tutorial of the curses (ncurses) library family, including the panel, menu, and form libraries.  Curses (which stands for cursor options) is a library used for making terminal/text based user interfaces.  Ncurses was a later clone of the System V library that substantially extends the original library.  What is contained here is a desciption of how to use ncurses, as you would not likely want to use the original library.

Overview: this tutorial is a description of the ncurses library and notes on how to use its interfaces, as well as details about its operation.  For starters, ncurses is a wrapper for the complex terminfo detail that abstracts away the particulars of each terminal type such that you can manage input/output.  In short, ncurses is a library that supports the creation of user interfaces on text terminals.

Note that this tutorial covers almost everything you would ever want to know (or need) about ncurses and is designed for the C programming language.  There is support other languages (e.g., python) and as I understand it the concepts laid out herein translate to these other languages implementations as well.  Also note that there are a number of additional functions that are useful but less frequently used. These are most directly documented in the associated man pages.

This tutorial was created from the library man pages, NCurses Programming HOWTO, and random sources on the Internet as well as personal experimentation and trial and error.  Thanks to the many folks who contributed to curses (the original library), ncurses and the numerous helpful documents, pages and posts.

**Patick McDaniel (mcdaniel@cs.wisc.edu), Jan 2024**

## Central concepts: windows and coordinates

**Windows** - windows are the basic structure that contains output (and can receive input) within the ncurses system.  By default, you get a default window which is the size of terminal you are using (e.g., 80x25).  There is always the default window, called stdscr that encompasses the entire terminal.  You can also create new windows with their own dimensions as needed.  The key here is that windows provide an sectioned area of the screen that can be managed independently of others.   The central attributes of a window is that they are defined by the location (with an origin upper left corner) and dimensions describing the number of rows and columns.

Windows can also have subwindows (and they in turn can have further subwindows), which are windows that exist within the space of a parent window.  Thus, windows structures support the notion of a hierarchy of windows starting at the root window.

**Coordinates and the cursor** - all windows are defined by the coordinate system containing _y_ rows and _x_ columns.  For historical reasons, the coordinates are always presented and passed as _row_ first and _col_ second.  Weirdly, this translates to (y, x) which is the opposite of what we would do in a coordinate system.  A key notion here is that for all windows have a __cursor__ that is at a position within the window coordinate system.  The cursor changes whenever you move it explicitly or add characters to the screen/window.   For example, if you add one character to the window it will move one position to the right.

## Building with ncurses

To build, you need to include the base library, as well as any libraries you want to use for your application.  If you use everything listed here, you will want to use:

    #include <ncurses.h>
    #include <panel.h>

To link, you will need to add the link directives associated with the libraries.  For example.

    g++ -o ctest ctest.o -lncurses lpanel -lmenu

## Environment life-cycle

There are three stages of the lifecycle of the curses environments:

1. **startup**, which is performed using the initscr() function, which accept no parameters and returns nothing.  The function setup the library and go into the environment.  This is required before you can do anything with the terminal.  Note that you will also want to set several state variables as part of this setup (see next section).

      initscr(); // Refreshes stdscr

2. **running**, where we have some elements (see below) that contain text and other stuff are written to the physical interface when refresh() is called.  Basically, refresh() re-displays all of the data on the screen and wresfresh() re-displays on the window.  

        refresh();     // Refreshes stdscr
        wrefresh(win); // Refreshes the window

3. **shutting down the interface**.  This is done with the endwin() function that frees all of the internal data structures and returns the interface to the normal terminal.  This is super important to do, because if you don't the terminal will be in a messed up (and largely non-functional) state after exit.

        endwin();

## Flags/initialization state and related functions.

1. **Input buffering state**.  Normally the terminal buffers all of the input coming from the user.  You can turn this off with raw() and cbreak(), which return the character back to the program immediately.  This is most useful state when trying to program the UI.  The difference between these is that raw() returns the special control characters CTRL-Z and CTRL-C to the program, whereas cbreak() retains their function.  cbreak is what you will normally use

        cbreak();

2. **Character echo state**.  Implemented by the echo() and noecho() functions, This indicates where the terminal will echo the input character to the current cursor location.  Most of the time you will want to turn this off.

        noecho();

3. **Extended character enabling**.  Implemented by the keypad function(), this enables things like function keys and the number pad (plus import characters like the arrow keys).  This is also super important.  The function accepts two parameters, one is the screen and the other is a boolean value indicating whether to enable them (TRUE) or to disable them (FALSE).

        keypad(stdscr, TRUE);

## Windows and cursor functions

As mentioned above, windows are the basic unit of UI interface.  You can create as many of these and they can overlap, stack, etc. as you need when you design the your interface.  In fact, the key to designing your interface is to see what windows you need, when to create, where to place, move and decorate, and when to destroy them.  The central functions are:

1. **newwin()** - this creates a new window at a location with specified dimensions.  Again, remember that the coordinates and dimensions are always listed as row, column.

        WINDOW local_win = newwin(height, width, row, col);

2. **subwin()/derwin()** - these functions create subwindows.  The difference between the two is that the coordinates passsed to subwin are relative to the screen, and the coordinates passed to derwin are relative to the parent window.

        WINDOW sub = subwin(win, rows, cols, y, x); // Relative to scr 
        WINDOW sub2 = derwin(win, rows, cols, y, x); // Relative to win

2. **delwin()** - this destroys the window and but *does not* s its contents disappear (for this, see clear() and erase() below).  This is just the free of the window process.

        wclear(win);
        wrefresh(win);
        delwin(win);

    One note is that if you want to clear the contents of the window, use clear()/erase() [see below], then refresh(), then delete.

3. **getmaxxy()**.  This tells you the size of the current window.  This is also useful when you want to know the size of the screen.   This is done through the MACRO getmaxxy() (which is weird because you are kinda passing by reference but don't need a & in front of it).

        int x, y, sy, sx;
        getmaxyx(win, y, x);
        getmaxyx(stdscr, sy, sx); // Gets the size of the window

    Note that there two values assigned when the initscr() is called, LINES and COLS, which are given values for the size of the window.  These are not reset of the window size changes are are generally more unreliable to use.

4. **border()/wborder()/box()** - this creates a border around the window using the default horizontal and vertical lines and corners (unless you change them).  Basically, this gives this a nice look about the screen and creates a kind of window look you would see in a graphical user interface.  The arguments paint the outside edge of the window with characters that you can customize (where 0 means use default).

        border(ls,  rs, ts, bs, tl, tr, bl, br);
        border(0, 0, 0, 0, 0, 0, 0, 0);
        wborder(win, ls,  rs, ts, bs, tl, tr, bl, br);
        box(win, 0, 0);

    Where:

        ls  Starting-column side (default : ACS_VLINE)
        rs  Ending-column side (default: ACS_VLINE)
        ts  First-line side (default: ACS_HLINE)
        bs  Last-line side (default: ACS_HLINE)
        tl  Corner of the first line and the starting column (default: ACS_ULCORNER)
        tr  Corner of the first line and the ending column (default: ACS_URCORNER)
        bl  Corner of the last line and the starting column (default: ACS_LLCORNER)
        br  Corner of the last line and the ending column (default: ACS_LRCORNE)

    In reality, however you wil almost never customize the edges.  While not borders, note that there are additional functions for creating horizontal and vertical lines, mvhline() and mvvline().

5. **erase()/werase()/clear()/wclear()** - these functions erase the contents of the screen or window.  The difference between erase and clear is that the latter calls clearok(), which cause a complete clear and redraw of the entire screen following the next wrefresh().  In almost all respects the latter functions lead to the best behavior.

        wclear(win);

5. **getxy()/wgetxy()/move()/wmove()** - this gets and sets the location of the current cursor.  The get functions are macros, so you don't have to pass things by reference.

        int x, y;
        wgetxy(win, y, x)
        wmove(win, 10, 10);

    You can also change how the cursor looks on the display with the function curs_set(), which has one of three values, 0=invisible, 1=normal, and 2=strong.

        curs_set(2); // Show the cursor as a strong visual

## Input functions

There are a couple of input functions that depend on the modality of the input.  The main ones are:

1. **Character input**.  getch() is the main function that receives keyboard input.  This will return with the character (which is actually an integer) typed into the keyboard as it happens, assuming you have disabled the buffering (see input buffering above).  The functions return ERR (integer define from ncurses) and OK if it worked.

        int ch = getch();
        ch = wgetch()
        ch = mvgetch();
        ch = mvwgetch();

    What is returned is an integer which will either be an ASCII (0-255) number or some extended character such as function and arrow keys.  The basic ones are defined in the include file and can be used in code pretty easily.  

        int ch = getch();
        if ( ch == KEY_UP ) {
            mvwprintw(win, 7, 0, "You pressed the up arrow key");
        }

    Note that ungetch() puts a character back onto the input queue, and the has_key() function determines if the current terminal supports a specific terminal.   Also be careful to set noecho() and keypad() to make sure you don't get echoing behavior and that extended characters are supported.

2. **String input**.  getstr() and its variants are the functions used to get input by string format.  What happens is that it receives input until it receives a RETURN key, then saves it to the string.  Note that like sscanf(), the non-space declared versions can be unsafe and lead to vulnerabilities and crashes.  So, use the "n" varients.  The functions return ERR (integer define from ncurses) and OK if it worked.

        char str[80];
        getstr(str);
        getnstr(str, 79);
        wgetstr(str);
        wgetnstr(str, 70);
        mvgetstr(y, x, str);
        wgetnstr(win, y, x, str, 70);

    Note that there are a collection of functions scanw() and vscanw() which mirror the functionality of scanf() and vscanf().  These function do a getstr() of some type then run the scan over the resulting string.

3. **Mouse input**.  Dealing with the mouse is a pretty straightfoward exercise.  The idea is that you register for specific events, then getch() indicates that mouse events are available, and getmouse() tells you what the event it.  Breaking this down, the first function to understand is mousemask():

        mmask_t mask, oldmask;
        mask = BUTTON1_PRESSED | BUTTON_SHIFT | BUTTON2_CLICKED;
        mousemask(mask, &oldmask); // Assigns new mask and saves previous one
        mouseinterval(0); // Set minimum time between mouse events

    Here, this indicates to the curses library which mouse events to report.  There are lot of them, but the most frequently used ones are:

        BUTTON1_PRESSED          button 1 down
        BUTTON1_RELEASED         button 1 up
        BUTTON1_CLICKED          button 1 clicked
        BUTTON1_DOUBLE_CLICKED   button 1 double clicked
        BUTTON1_TRIPLE_CLICKED   button 1 triple clicked
        BUTTON2_PRESSED          button 2 down
        BUTTON2_RELEASED         button 2 up
        BUTTON2_CLICKED          button 2 clicked
        BUTTON2_DOUBLE_CLICKED   button 2 double clicked
        BUTTON2_TRIPLE_CLICKED   button 2 triple clicked
        BUTTON_SHIFT             shift was down during button state change
        BUTTON_CTRL              control was down during button state change
        BUTTON_ALT               alt was down during button state change
        ALL_MOUSE_EVENTS         report all button state changes
        REPORT_MOUSE_POSITION    report movement

    Note that the left button is BUTTON1 and the right is BUTTON2.  There are also BUTTON3 and BUTTON4 available for people with mice that support it.  For compatibility with different peoples's computers I tend to avoid using those.  Note tha a click is an UP-DOWN mouse event and a press is just DOWN.

    One other thing to know about dealing with the mouse is that it has a event resolution setting.  Basically, this is the minimum amount of time in milliseconds that mouse events can be reported--here, this acts as a damper for mouse events (to prevent your UI from constantly being overloaded with events).  The function mouseinterval() is used for setting this, where a value of 0 disables the damper, and -1 returns the current setting without changing it.
    
    Another interesting event is the ALL_MOUSE_EVENTS which reports everything, and REPORT_MOUSE_POSITION which reports mouse movement.  Both of those tend to generate a lot of events, so design your code accordingly if you wish to use them.  Note that getting the mouse position reporting can be challenging on some termainals.

    Getting and processing the mouse events is a two stage process.  The first part is pretty easy--you simply call getch() which returns KEY_MOUSE if a mouse event is avilable.  Thereafter you you call getmouse() to obtain the event.

        MEVENT event;
        ch = getch();
        if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate & BUTTON1_PRESSED) {
                    mvwprintw(win, 8, 0, "Left mouse button at %d, %d", event.x, event.y);
                    wrefresh(win);
                }
            }
        }

    The important part is that the mouse event structure has the state of the mouse event, its location, and and ID that is specific to the mouse that generated the event (this is useful when a system has multiple input devices, such as a mouse and a touchpad).  The structure looks like:

        typedef struct {
            short id;         // ID of event generating device
            int x, y, z;      // Location of the device in coordinate system
                              // Note that the 'z' coordinate is unused
            mmask_t bstate;   // State bits (see definitions above)
        }

    Note that when working with the mouse, you often have to figure out which window (or the main screen) you are working with.  To do this, you call the wenclose(), which tests where the a point in the screen coordinate space is within a window.  This returns true when it is and false when it is not.

        bool inwindow;
        inwindow = wenclose(win, event.y, event.x);

    One last handy set of functions converts window coordinates to screen coordinates and vice-versa.  These are the mouse_trafo() functions.

        // Convert screen -> window coordinates (returns false if not in window)
        int wx = event.x, wy = event.y, sx,sy;
        if (wmouse_trafo(win, &wy, &wx, FALSE) == TRUE) {  
            sx = wx; sy = wy;
            wmouse_trafo(win, &sy, &sx, TRUE);  // Convert screen -> window coordinates
        } 

## Output functions

There is a wide array of output functions. 

1. **printw()/wrprintw()/mvwprintw()** - this is the simplest of the output functions, and it outputs a string constructed using the printf formatting.  The output is put at the current of the cursor.

        printw("This goes to the cursor location on the stdssc");
        mvprintw(y, x, "This goes to specified location in stdscr");
        wprintw(iwn, "This goes to the cursor location in the window");  
        mvwprintw(win, y, x, "This goes a specified point in the window);

2. **addch()/waddch()/mvwaddch()** - this puts a character at the current cursor location.  The nice thing about this function is that the character can be decorated using attributes that do things like bolding and underlining.  This function has window and window/move variants.

        addch('a');
        addch(ch | A_BOLD | A_UNDERLINE);

    In addition to all of the normal characters there are a bunch of ascii supported extended characters sich as arrows, symbols, etc.  Much of these are callewd ACS names.  For a complete list, search curses.h for ACS_ and they are listed.  A couple examples are ACS_PI, ACS_RARROW, and ACS_DIAMOND.  You usese the #defines just as any normal character:

        waddch(ACS_DIAMOND | A_BOLD);

3. **addstr/waddstr/mvaddstr** - these place a string on the screen or window according to the normal conventions.  This is just like doing a addch() once for every character in the string.

        addstr("string to add");
        waddstr(win, "string to add to window");
        mvaddstr(y, x, "string to add at specific location);
        mvwaddstr(win, y, x, "string to add at specific location);

    One function that is useful in preventing ugly text caused by overwriting one string over another is clrtoeol(), which clears the text to the end of the line.  Use this when printing a variable length string over and over to prevent previous strings of longer lengths show the end of thier string.
    
## Attributes

 Attributes are decorators for characters in ncurses.  The idea is that you can add decorations onto characters that are placed into the interface.  There are a couple of ways to enable and use attributes, the two central ones are:

 1. **attron()/attroff()/wattron()/wattroff()** - this function enables or disables attributes.  Enabled attributes remain on until they are turned off.   Note that the attributes can be ORed as parameters in the attribute functions.  

        attron(A_REVERSE | A_BOLD);
        attroff(A_DIM | A_UNDERLINE);
        wattron(win, A_REVERSE | A_BOLD);
        wattroff(win, A_DIM | A_UNDERLINE);

    Note that all of these functions accept integers (often made from the #defines listed below).  If you want to be more proper, you will use the attr_t type.  In that case, you will want to use the attr_ varients.  Note that the last option (void *opt) is unused and should always be NULL.

        attr_t attr = A_REVERSE | A_BOLD;
        attr_on(attrs, NULL);
        attr_off(attrs, NULL);
        wattr_on(win, attrs, NULL);
        wattr_off(win, attrs, NULL);

2. **attrset()/wattrset()** - this function resets the attributes for the screen or window.  This is kind of like turning off all of the attributes and turning the new ones on.

        attrset(A_REVERSE | A_UNDERLINE);
        wattrset(win, A_NORNAL); // Just turns off all of the attributes

    Like the on/off functions above, there are attr_ varients that you can use.

        attr_t attr = A_REVERSE | A_BOLD;
        attr_set(attrs, NULL);
        wattr_set(win, attrs, NULL);

3. **chgat()/wchgat()** - changes the attributes of a set of characters from the cursor forward, where -1 means to the end of the line.

        chgat(6, A_REVERSE, NULL);
        chgat(-1, A_BOLD, NULL); // Makes everything to end of line bold

3. **attr_get()/wattr_get()** - This just gets the current attributes from the window/screen.  It returns a type that is attr_t (which is just a uint32_t) containing the OR of the current attributes and color schemes.  

        attr_t attr; // Attribute sets
        short pair; // Color scheme
        att_get(&attr, &pair, NULL);
        wattr_get(win, &attr, &pair, NULL);

5. **standend()/wstandend()** - These functions just turn off all of the attributes (e.g., returns the attributes to normal).

        standend();
        wstandend();

 The types of attributes that are available are: 

    A_NORMAL        Normal display (no highlight)
    A_STANDOUT      Best highlighting mode of the terminal.
    A_UNDERLINE     Underlining
    A_REVERSE       Reverse video
    A_BLINK         Blinking
    A_DIM           Half bright
    A_BOLD          Extra bright or bold
    A_PROTECT       Protected mode (old mode for VT terminals that prevents erase)
    A_INVIS         Invisible or blank mode
    A_ALTCHARSET    Alternate character set
    A_CHARTEXT      Bit-mask to extract a character
    COLOR_PAIR(n)   Color-pair number 

## Colors

The central idea for using colors is that you define foreground/background color pairs then use them just like normal decorations discussed in the previous discussion.  The way to think about this is that you turn on colors, define color pairs, then use them.  The following functions are the way you get this done.

1. **has_colors()** - this function checks to see if the terminal supports colors.  It returns TRUE if it does and FALSE if not.  It is a good idea to check this first, so you don't call a bunch of stuff you don't need to.

2. **start_color()** - this enables the use of colors in the terminal.  I am not aware of a way to disable colors once you have enabled them, but I am not clear on why anyone would want to do so (just don't use them).

3. **init_pair()** - this function allows you to create pairs of foreground/background colors.  What you need to know is that the library keeps 256 different colors and can have up to COLOR_PAIRS (a variable defined in ncurses.h, 65536 color pairs.  Each color is indexed by a number 0-255, where the following colors are predefined (in ncurses.h):

        COLOR_BLACK   0
        COLOR_RED     1
        COLOR_GREEN   2
        COLOR_YELLOW  3
        COLOR_BLUE    4
        COLOR_MAGENTA 5
        COLOR_CYAN    6
        COLOR_WHITE   7

    So, to create a pair you give it an index (< COLOR_PAIRS) and two colors.

        init_pair(42, COLOR_BLACK, COLOR_RED);

    You can also retrive the colors for a specific pair using the pair_content function.

        short pindex = 1, foreground, background;
        pair_content(pindex, &foreground, &background);

4. **init_color** - this function create a new color based on an RGB specification.  Note that the color schemes are scaled 0-1000, so some translation from 0-255 RGB numbers is required.  Essentially you give it an index (>7 to avoid overwriting existing colors), and an scaled RGB value to define the new color.  Assuming you have the normal (0-255,0-255,0-255) color scheme, you can use the following to create a new color (in this case, the official red color of the University of Wisconsin-Madison):

        #define RGB_TO_CURSES(x) ((int)((float)x*3.90625))  // 1000/256 conversion
        short badger_red = COLOR_WHITE + 1;
        init_color(badger_red, RGB_TO_CURSES(197), RGB_TO_CURSES(5), RGB_TO_CURSES(12));

    Now that you have this new color, you can add it to a pair and use it to decorate any text output.

        init_pair(43, badger_red, COLOR_WHITE);
        attr_on(COLOR_PAIR(43));
        mvprintw(3, 0, "This is now a special UW-Madison color!", sy, sx);

    You can retrive the settings for a specific color using the color_content function.

        short cindex = 43, red, green, blue;
        color_content(cindex, &red, &green, &blue);

## Panels

At this stage you have enough to do some basic UI.  The truth is that the update process can get a little wonky and the bookkeeping necessary to manage all of this is pretty complication and very difficult to debug.  While technically not part of the curses library, the Panels library is designed to address these challenges and leave you to work directly on the important stuff of your application.

Panels actually work the way that you might expect windows to work (and actually, they are a code construct much closer to the windows interfaces we are used to in X11, OSX, and Windows).  The idea is that they represent a set of windows that are stacked on the screen.  When displayed (using update_panels(), then doupdate()), you see all of the top one, all of the next one down the stack minus whatever parts are covered by the top one, etc.  You can also move panels around, show and hide them, etc. 

The basic functions are:


1. **new_panel()** - this creates a new panel based on a previously defined window.

        WINDOW *win = new_window(10, 10, 20 20);
        WINDOW *win2 = new_window(10, 10, 25, 25);
        PANEL *pan = new_panel(win);
        PANEL *pan2 = new_panel(win2);

2. **update_panel()** - update the stacking order, where each new panel will be "on top" of the others defined.  In the case of the previous example, win2 will be on top of win.

        update_panels();

3. **doupdate()** - this shows the panels on the screen.  It is a bit strange to see this as being seperate from update_panels, but it separates the shuffling and reworking of the panels on the virtual screen  from the actual display operation.

        doupdate();

4. **top_panel()/bottom_panel** - moves the panel to the top or bottom of the stack.

        top_panel(pan);
        bottom_panel(pan2);

    Also, there are panel_above() and panel_below() which allows you to travese the panel stack.

5. **set_panel_userptr** - this allows an arbitrary pointer to be associated with a panel.  Often this will be some other UI object or application structure.

        MyObject *obj = new MyObject(), *obj2;
        set_panel_userptr(pan, obj);
        ...
        obj2 = panel_userptr(pan);

6. **show_panel()/hide_panel()** - this turns on and off the display state of the panels.  This is useful for keeping interface objects around that you only need once in a while.

        hide_panel(pan);
        show_panel(pan);
        int hidden = panel_hidden(pan); // Sets flag to TRUE if hidden

7. **move_pane()** - this moves the panel around the interface.

        move_panel(pan, 30, 30);

8. Finally, you can delete a panel using the final del_panel() function.

        del_panel(pan);

## Menus

Menus are an interface object that you can use to allow the selection of multiple ements.  This is just like what you would see in normal UI interfaces, but just in text.  The lifecycle of menus is pretty straightforward, you create the menus/items, post them, then monitor for menu events.  When you are done, you unpost and free the menu items and you are done.

1. **new_menu()/new_item()/post_menu()** - this allows you to create a menu, then create and attach items onto the menu.  Whe you are done, post the menu so the UI knows to start creating it.

        int i;
        const char * itemtext[5][2] = {
            "Item 1", "Description 1",
            "Item 2", "Description 2",
            "Item 3", "Description 3",
            "Item 5", "Description 5"
        };
        ITEM **testitems = (ITEM **)calloc(5, sizeof(ITEM *));
        for (i=0; i<5; i++) {
            testitems[0] = new_item(itemtext[i][0], itemtext[i][1]);
        }
        MENU *testmenu = new_menu(testitems);
        post_menu(testmenu);

    Note that the item text and description pass is saved by reference (by pointer), so you need to be carful to NOT have that data somewhere where it will go out of scope, e.g., on the stack.   Best practices seems to be to have this as some kind of global, file, or class static data.

    Note that the new_item() function can be used in conjunction with set_item_userptr() and later item_userptr() which allows you to set an opaque pointer for used.

2. **item_opts_off()/items_opts_on()** - you can also set options on menu items.  There is only one of these, however, the ability to turn on and off selectability of a menu item, O_SELECTABLE.

        item_opts(testitems[2], O_SELECTABLE);
        item_opts_on(testitems[3], O_SELECTABLE);  

2. **set_menu_fore()/set_menu_back()/set_menu_grey()** - these functions allow you to set the colors that will be displayed as part of the menu, where the colors represent foreground, background, and non-selectable.

        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_BLUE, COLOR_BLACK);
        init_pair(3, COLOR_WHITE, COLOR_BLACK);
        set_menu_fore(testmenu, COLOR_PAIR(1) | A__REVERSE);
        set_menu_back(testmenu, COLOR_PAIR(2));
        set_menu_grey(testmenu, COLOR_PAIR(3));

4. **menu_driver()** - this is the thing that makes the menu do things.  Specifcally, you tell the menu driver what operation to do, and it reflects it in the interface.

    The menu driver accepts:

        REQ_LEFT_ITEM   // Move left
        REQ_RIGHT_ITEM  // Move right
        REQ_UP_ITEM     // Move up
        REQ_DOWN_ITEM   // Move down
        REQ_SCR_ULINE   // Scroll up
        REQ_SCR_DLINE   // Scroll down
        REQ_SCR_DPAGE   // Scroll down (page)
        REQ_SCR_UPAGE   // Sroll up (page)
        REQ_FIRST_ITEM  // Goto first item
        REQ_LAST_ITEM   // Goto last
        REQ_NEXT_ITEM   // Got next time
        REQ_PREV_ITEM   // Previous item
        REQ_TOGGLE_ITEM // Select/de-selecct item

    There is also a search mechanism so that rather than move between items, we can user a search term and then move to the item(s) that it matches.  This provides a nice interface to do a quick search through the list.  The idea is that the menu, then walk through that match.  Each menu has a pattern buffer that you can add to simply by giving it the menu_driver() function.
    
    The commands are:

        REQ_CLEAR_PATTERN // Clear pattrn buffer.
        REQ_BACK_PATTERN  // Delete prev char from pattern
        REQ_NEXT_MATCH    // Move to next match
        REQ_PREV_MATCH    // Move to previous match

    Putting this all together, here is a simple menu handing code snippet.

        ch = getch();
        switch (ch) {
            case KEY_DOWN:
                menu_driver(testmenu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(testmenu, REQ_UP_ITEM);
                break;
            default:
                menu_driver(testmenu, ch); // Add to search buffer
                menu_driver(testmenu, REQ_NEXT_MATCH);
                break;
        }

5. **set_menu_win()/set_menu_sub()** - this associates the menu with a window.  You must set both of these because the window is the thing that has the menu (like a large window), and the subwindow is a window containing the menu iteself. 

        set_menu_win(basic_menu, win);
        set_menu_sub(basic_menu, derwin(win, 6, 38, 3, 1));

    

5. **unpost_menu()/free_menu()** - this allows you to clear the menu from the interface (unpost_menu(), and then cleanup/free the menu associated from its use:

        unpost_meny(testmenu);
        free_menu(testmenu);

6. **set_menu_format()/set_menu_options()** - There are also lots of other things like multi-column format via set_menu_format(), as well as menu options, set_menu_options(), allow you to do a lot of cool menu tricks like multiple columns and multi-section, etc.

## Forms

The forms library allows you to create interactive interfaces structured as fields that can are allocated on single canvas such as a window.  In many respects the creation of a form is a lot like that of a menu.  You begin by creating fields, annotating them, posting, calling the form driver, and then shutting it down.

1. **new_field()/new_form()** - These create the base objects.  For new_field(), you pass the number of rows, number of columns, row location, column location, number of offscreen rows and number of buffers.  new_form() accepts an array of fields whose last element is NULL.

        FORM *test_form;
        FIELD *test_fields[3];
        test_fields[0] = new_field(1, 10, 0, 0, 0, 0);
        test_fields[1] = new_field(1, 10, 1, 0, 0, 0);
        test_fields[2] = NULL;
        test_form = new_form(test_fields);

    You can also get the parameters of a field using the field_info() function.  One other key thing to note is that labels are simply fields that are not editable (see O_ACTIVE, O_EDIT below).

2. **set_form_win()/set_form_sub()/sclae_form()** - These functions setup the form and subwindow for a particular form.  The idea is the window is where the whole display goes (with framing, etc.), and the subwindow is where the fields that are selectable/visible go. The scale_form() function tells you what the minimum rows and columns you need in the subwindow to fir the interface.

        int rows, cols;
        scale_form(basic_form, &rows, &cols);
        set_form_win(basic_form, win);
        set_form_sub(basic_form, derwin(win, rows+1, cols+1, 1, 1));

2. **set_field_fore()/set_field_back()/set_field_just** - You can set lots of display attributes for a field using a set of attribute functions.  Some of the more interesting ones are:

        set_field_back(test_fields[0], COLOR_PAIR(1));  // Only sets back
        set_field_fore(test_fields[0], COLOR_PAIR(2));  // Only sets fore
        set_field_just(test_fields[0], JUSTIFY_CENTER);

    The super wierd thing to remember with colors and fields is that the set_field_fore() only sets foreground portion, and the set field back sets only the background portion.  All other attributes work same as above, however.

3. **set_field_opts()/field_opts_on()/field_opts_off()** - These functions are used to set options for the fields, where set assigns the options, and off turns them on and off.

        set_fields_opts(test_field[0], O_ACTIVE);
        fields_opts_on(test_fields[1], O_BLANK);
        field_opts_off(test_fields[0], O_EDIT|O_ACTIVE); // Creates a label

    Turning on and off fields the opts_on() is safer, so it is recommended.  Where the field options include

        O_VISIBLE   // Show the field or not on screen
        O_ACTIVE    // Controls whether the fields is visited (lables=0)
        O_PUBLIC    // Show the data while editing or typing
        O_EDIT      // Turns editing on and off
        O_WRAP      // Enables wrapping 
        O_BLANK     // Erase the contents when first character typed
        O_AUTOSKIP  // Move automatically to next field when field is full
        O_NULLOK    // Call validation (see below)
        O_PASSOK    // Only call validation when edited
        O_STATIC    // Controlls whether field will stretch to data size

2. **post_form()/unpost_form()** - the posts the form to the screen and removes it.  Note that unpost removes it from the form management system, which is different than making it invisible.

        post_form(test_form);
        ...
        unpost_form(test_form);

4. **form_driver()** - this perfortms the operation of the forms, specifically adding characters, moving between fields, etc.

        form_driver(test_form, REQ_NEXT_FIELD);
        form_driver(test_form, ch); // Add character to the buffer

    You can also just assign a specific field as having the focus using the set_current_field() function.

5. **set_field_buffer()/set_field_status()/get_field_status()** - These functions allow you to set and retrieve the contents of a buffer and determine the edit status of a field.  Note that the forms library supports fields with multiple buffers, but I generally have not found a good use for them.

6. **set_current_form()/current_field()/field_index()** - These functions set or get the focus of the field on the interface.  A return code of -1 (or NULL in the case of returned pointers) indicates an error.

        FIELD *cur = current_field(form); // Get the current field and index
        int fld = field_index(cur);
        
        set_current_field(form, field); // Set the current field