CC = gcc
CFLAGS = -Wall -O3 -g

EXE =

XLIBS = -lseerd -L/usr/X11R6/lib -Wl,-rpath,/usr/local/lib /usr/local/lib/liballeg.so -lXext -lX11 -lm

LIBS = $(XLIBS)


all : mapedit$(EXE)


%.o : %.c
	$(COMPILE.c) -o $@ $<


OBJS = mapedit.o report.o df.o script.o hash.o tiles.o dirty.o \
	maptiles.o convtbl.o rpx.o

mapedit$(EXE) : $(OBJS)
	$(CC) -o $@ $^ $(LIBS)
