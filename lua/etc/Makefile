# makefile for Lua etc

LUA= ..

include $(LUA)/config

LIBLUA=$(LIB)/liblua.a
ALL= bin2c min trace noparser lua.def

all:
	@echo 'choose a target:' $(ALL)

bin2c:	bin2c.c
	$(CC) $(CFLAGS) -o $@ $@.c

min:	min.c $(LIBLUA)
	$(CC) $(CFLAGS) -o $@ $@.c -L$(LIB) -llua

trace:	trace.c $(LIBLUA)
	$(CC) -g $(CFLAGS) -o $@ $@.c -L$(LIB) -llua -llualib -lm

noparser: noparser.c
	$(CC) $(CFLAGS) -I$(LUA)/src -o $@.o -c $@.c

def:	lua.def

lua.def: $(INC)/lua.h def.lua
	$(BIN)/lua def.lua < $(INC)/lua.h > $@
	# If you want to export auxlib and the standard libs as well, do:
	# cat $(INC)/l*lib.h | $(BIN)/lua def.lua > lualib.def

stdcall:
	mkdir -p Stdcall/lua/src/lib Stdcall/src/lib 
	grep -l _API $(LUA)/src/*.[ch] $(LUA)/src/*/*.[ch] | xargs -n1 -i echo $(BIN)/lua stdcall.lua '<{}' '>Stdcall/lua/{}'

flat:
	cd ..; mkdir flat; mv include/*.h src/*.[ch] src/*/*.[ch] flat

$(LIBLUA):
	cd ../src; $(MAKE)

clean:
	rm -f $(ALL) a.out core *.o lualib.def
