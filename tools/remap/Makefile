CC := gcc
LUADIR := ../../lua-5.0-alpha
CFLAGS := -Wall -g -I $(LUADIR)/include
LDFLAGS := -s
LDLIBS := `allegro-config --libs`		\
	  $(LUADIR)/lib/liblua.a		\
	  $(LUADIR)/lib/liblualib.a

remap: remap.o 

remap.o: remap.c remap.inc

remap.inc: remap.lua
	(echo -e '/* generated from remap.lua */\n'; \
	echo 'static const char *remap_script = "'; \
	cat remap.lua | sed 's/\\/\\\\/g' | sed 's/"/\\"/g'; \
	echo '";') > remap.inc


clean:
	rm -f remap.o remap
