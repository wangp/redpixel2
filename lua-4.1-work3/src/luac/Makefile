# makefile for Lua compiler

LUA= ../..

include $(LUA)/config

INCS= -I$(INC) $(EXTRA_INCS) -I..
OBJS= dump.o luac.o print.o lopcodes.o
SRCS= dump.c luac.c print.c luac.h ldumplib.c

T= $(BIN)/luac

all: $T

$T: $(OBJS) $(LIB)/liblua.a
	$(CC) -o $@ $(MYLDFLAGS) $(OBJS) -L$(LIB) -llua

$(LIB)/liblua.a:
	cd ..; $(MAKE)

lopcodes.o:	../lopcodes.c ../lopcodes.h
	$(CC) $(CFLAGS) -DLUA_OPNAMES -c -o $@ ../lopcodes.c

clean:
	rm -f $(OBJS) ldumplib.o $T

co:
	co -q -f -M $(SRCS)

klean:	clean
	rm -f $(SRCS)
