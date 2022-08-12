NAME 	= cthulhu

LIBS 	= -lcurses -ltermcap -lc
LDFLAGS	=

#CFLAGS	= -g -DTERMCAP -DSYSV
#CFLAGS	= -g -xenix -DTERMCAP
#CFLAGS	= -ql -DPROTO -DTERMCAP

# For modern Linux environments:
CFLAGS	= -g -DTERMCAP -DSYSV
CC	= gcc

SRC =	clock.c combat.c end_game.c errors.c file_func.c graphics.c heal.c \
	init.c main.c misc.c monsters.c objects.c spell.c start_game.c  \
	travel.c

OBJ =	clock.o combat.o end_game.o errors.o file_func.o graphics.o heal.o \
	init.o main.o misc.o monsters.o objects.o spell.o start_game.o \
	travel.o

$(NAME):
	$(CC)  -o $(NAME) $(LDFLAGS) $(CFLAGS) $(OPTIONS) $(SRC) $(LIBS)
	@chmod 2711 $(NAME)
	@ls -ls $(NAME)

proto: 
	rm -f proto.h

proto.h:
	$(CC) -Zg -p1 /lib/p1 $(SRC) > proto.h

all: $(Name)

clean:
	rm -f $(OBJ)
	rm -f proto.h
