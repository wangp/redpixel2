CC = gcc
CFLAGS = -Wall -O3 
LDFLAGS = 
LIBS = -lalleg



ifdef DJDIR
PLATFORM = djgpp
EXE = .exe
else
PLATFORM = linux
EXE = 
endif



all: libwum.a example$(EXE)



ifeq "$(PLATFORM)" "djgpp"
DJDIR_U = $(subst \,/,$(DJDIR))
DJDIR_D = $(subst /,\,$(DJDIR))
LIB_DEST = $(DJDIR_U)/lib/libwum.a
INC_DEST = $(DJDIR_U)/include/wumpus.h

$(LIB_DEST): libwum.a
	cp $< $@

$(INC_DEST): wumpus.h
	cp $< $@
endif



ifeq "$(PLATFORM)" "linux"
PREFIX = /usr/local
LIB_DEST = $(PREFIX)/lib/libwum.a
INC_DEST = $(PREFIX)/include/wumpus.h
endif



%.o: %.c
	$(COMPILE.c) -o $@ $<
	
	
libwum.a: wumpus.o hash.o	
	ar rs $@ $^
	
	
example$(EXE): example.o libwum.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	


INSTALL_FILES = $(LIB_DEST) $(INC_DEST)


install: $(INSTALL_FILES)


uninstall: $(INSTALL_FILES)
	rm -fv $(INSTALL_FILES)


clean:
	rm -f *.o example$(EXE) core libwum.a
