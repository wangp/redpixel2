# makefile for Lua etc

LUA= ..

include $(LUA)/config

LIBLUA=$(LIB)/liblua.a
ALL= bin2c min trace lua.def lua3

x:
	@echo 'choose a target:' all $(ALL)

all: $(ALL)

bin2c:	bin2c.c
	$(CC) $(CFLAGS) -o $@ $@.c

min:	min.c $(LIBLUA)
	$(CC) $(CFLAGS) -o $@ $@.c -L$(LIB) -llua

trace:	trace.c $(LIBLUA)
	$(CC) $(CFLAGS) -o $@ $@.c -L$(LIB) -llua -llualib -lm

def:	lua.def

lua.def: $(INC)/lua.h
	$(BIN)/lua def.lua < $(INC)/lua.h > $@
	# If you want to export the debug interface and the standard libs, do:
	# cat $(INC)/l*.h | $(BIN)/lua def.lua > $@

lua3:	$(LIBLUA)
	$(CC) $(CFLAGS) -o $@.o -c $@.c
	$(AR) $(LIBLUA) $@.o
	$(RANLIB) $(LIBLUA)

stdcall:
	mkdir -p Stdcall
	grep -l _API $(LUA)/src/*.[ch] $(LUA)/src/*/*.[ch] | xargs -n1 -i echo $(BIN)/lua stdcall.lua '<{}' '>Stdcall/{}'

flat:
	cd ..; mkdir flat; mv include/*.h src/*.[ch] src/*/*.[ch] flat

$(LIBLUA):
	cd ../src; $(MAKE)

clean:
	rm -f $(ALL) a.out core *.o
