MF=	Makefile

CC=	gcc -mms-bitfields -Wall -g 
CFLAGS= -c -IC:\gtk\include -IC:\gtk\include\gtk-2.0 -IC:\gtk\include\cairo -IC:\gtk\include\gdk -IC:\gtk\include\glib-2.0 -IC:\gtk\lib\glib-2.0\include -IC:\gtk\include\pango-1.0 -IC:\gtk\lib\gtk-2.0\include -IC:\gtk\include\atk-1.0 -IC:\gtk\include\gdk-pixbuf-2.0

LFLAGS=	-lm -fastsse
EXE=	main

SRC=	main.c	game.c

#
# No need to edit below this line
#

.SUFFIXES:
.SUFFIXES: .c .o

OBJ=	$(SRC:.c=.o)

.c.o:
	$(CC) $(CFLAGS) -c $<

all:	$(EXE)

$(EXE):	$(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LFLAGS)

$(OBJ):	$(MF)

clean:
	rm -f $(OBJ) $(EXE) core
