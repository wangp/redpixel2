ALLEGRODIR := $(HOME)/cvs/allegro40/allegro
LUADIR := ../../lua-5.0-alpha

CC := gcc
CFLAGS := -Wall -g -I$(ALLEGRODIR)/tools -I$(LUADIR)/include
SOURCES := rpxutil.c
LDLIBS := $(ALLEGRODIR)/lib/unix/libaldat.a `allegro-config --libs` \
	$(LUADIR)/lib/liblua.a $(LUADIR)/lib/liblualib.a

ifdef NO_LOADPNG
	CFLAGS += -DNO_LOADPNG
else
	CFLAGS += -Iloadpng
	SOURCES += loadpng/loadpng.c
	LDLIBS += -lpng
endif

rpxutil: $(SOURCES)
	$(CC) $(CFLAGS) -o rpxutil $(SOURCES) $(LDLIBS)
