ALLEGRO = $(HOME)/allegro
CFLAGS = -Wall -g -I$(ALLEGRO)/tools
LOADLIBES = $(ALLEGRO)/lib/unix/libaldat.a `allegro-config --libs` -llua

ifdef NO_LOADPNG
	CFLAGS += -DNO_LOADPNG
else
	LOADLIBES += -lldpng -lpng
endif

rpxutil: rpxutil.c
