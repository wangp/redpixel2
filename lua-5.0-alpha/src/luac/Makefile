# makefile for Lua compiler

LUA= ../..

include $(LUA)/config

INCS= -I$(INC) $(EXTRA_INCS) -I..
OBJS= dump.o luac.o print.o lopcodes.o
SRCS= dump.c luac.c print.c luac.h ldumplib.c

T= $(BIN)/luac

all:	$T

$T:	$(OBJS) $(LIB)/liblua.a
	$(CC) -o $@ $(MYLDFLAGS) $(OBJS) -L$(LIB) -llua -llualib $(EXTRA_LIBS)

# luac needs opcode names
lopcodes.o:	../lopcodes.c ../lopcodes.h
	$(CC) -o $@ -c $(CFLAGS) -DLUA_OPNAMES ../lopcodes.c

ldumplib:	ldumplib.o
	$(AR) $(LIB)/liblualib.a ldumplib.o
	$(RANLIB) $(LIB)/liblualib.a

ldumplib.o:	ldumplib.c dump.c
	$(CC) -o $@ -c $(CFLAGS) ldumplib.c

$(LIB)/liblua.a:
	cd ..; $(MAKE)

clean:
	rm -f $(OBJS) ldumplib.o $T

co:
	co -q -f -M $(SRCS)

klean:	clean
	rm -f $(SRCS)
