#
# File          : Makefile
# Description   : Build file for curses tutorial
# Created       : Thu Jan  4 08:41:48 EST 2024
# By            : Patrick Mcdaniel

#
# Project Protections

ctest : ctest.o
	gcc -o $@ ctest.o -lncurses -lpanel -lmenu -lform -lm

clean : 
	rm -f ctest ctest.o
