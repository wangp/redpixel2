CC := gcc
CFLAGS := -Wall -g
LDFLAGS := -s
LDLIBS := `allegro-config --libs` -llua -llualib

remap: remap.o 

remap.o: remap.c remap.inc

remap.inc: remap.lua
	(echo -e '/* generated from remap.lua */\n'; \
	echo 'static const char *remap_script = "'; \
	cat remap.lua | sed 's/\\/\\\\/g' | sed 's/"/\\"/g'; \
	echo '";') > remap.inc


clean:
	rm -f remap.o remap
