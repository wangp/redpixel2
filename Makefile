#----------------------------
#	Compiler options
#----------------------------

CC = gcc
CFLAGS = -Wall -O3 -g

COMMONFLAGS = -Isrc
DOSFLAGS = $(COMMONFLAGS)
XFLAGS 	= $(COMMONFLAGS)
DOSLIBS = -lseerd -lalleg
XLIBS 	= -lseerd -L/usr/X11R6/lib -Wl,-rpath,/usr/local/lib /usr/local/lib/liballeg.so -lXext -lX11 -lm


#----------------------------
#	Object dependencies
#----------------------------

COMMONOBJS = mapedit.o report.o wrapper.o script.o hash.o tiles.o dirty.o \
	     maptiles.o convtbl.o rpx.o export.o exalleg.o

DOSOBJS = $(addprefix dosobj/,$(COMMONOBJS))
XOBJS   = $(addprefix xobj/,$(COMMONOBJS))

AUTOEXP_SRC = defs.h mapedit.h script.h tiles.h wrapper.h object.h
    
AUTOEXP_DEPS = $(addprefix src/,$(AUTOEXP_SRC))


#----------------------------
#	Targets
#----------------------------

.PHONY : gen clean xclean dosclean clearbackups dos X all

all : 
	@echo "Pick a target, any target:"
	@echo "	   make { dos X }"

dosobj/%.o : src/%.c
	$(COMPILE.c) $(DOSFLAGS) -o $@ $<

xobj/%.o : src/%.c
	$(COMPILE.c) $(XFLAGS) -o $@ $<

mapedit.exe : $(DOSOBJS)
	$(CC) $(DOSFLAGS) -o $@ $^ $(DOSLIBS)

mapedit : $(XOBJS)
	$(CC) $(XFLAGS) -o $@ $^ $(XLIBS)


#----------------------------
#	Auto-exports
#----------------------------

scripts/_imports.sh src/_export.c : $(AUTOEXP_DEPS) tools/autoexp.c
	./autoexp --output-seer-hdr=scripts/_imports.sh --output-c-src=src/_export.c $(AUTOEXP_DEPS)
	
gen : scripts/_imports.sh src/_export.c

autoexp autoexp.exe : tools/autoexp.c
	$(CC) $(CFLAGS) -o $@ $<
	

dos : autoexp.exe gen mapedit.exe
X : autoexp gen mapedit


#----------------------------
#	Misc targets
#----------------------------

dosclean :
	rm -f dosobj/* mapedit.exe autoexp.exe

xclean : 
	rm -f xobj/* mapedit autoexp.exe
	
clearbackups :
	rm -f *~ scripts/*~ src/*~

clean : dosclean xclean
	rm -f core seer-debug.log

