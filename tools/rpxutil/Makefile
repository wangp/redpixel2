ALLEGRODIR := $(HOME)/cvs/allegro
LUADIR := ../../lua-4.1-work3

CC := gcc
CFLAGS := -Wall -g -I$(ALLEGRODIR)/tools -I$(LUADIR)/include
SOURCES := rpxutil.c
LDLIBS := $(ALLEGRODIR)/lib/unix/libaldat.a `allegro-config --libs` $(LUADIR)/lib/liblua.a

ifdef NO_LOADPNG
	CFLAGS += -DNO_LOADPNG
else
	CFLAGS += -Iloadpng
	SOURCES += loadpng/loadpng.c
	LDLIBS += -lpng
endif

rpxutil: $(SOURCES)
	$(CC) $(CFLAGS) -o rpxutil $(SOURCES) $(LDLIBS)
