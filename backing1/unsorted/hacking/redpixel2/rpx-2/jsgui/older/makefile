CC = gcc
CFLAGS = -Wall -O3 -g
EXE = .exe
LIBM = 

all : example$(EXE)

%.o : %.c
	$(COMPILE.c) -o $@ $<

example$(EXE) : jsgapi.o jsgloop.o jsgwm.o jsgwdraw.o main.o
	$(CC) $(CFLAGS) -o $@ $^ -lalleg -ljs $(LIBM)

clean :
	rm -f *.o example$(EXE)
	
