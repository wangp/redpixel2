#----------------------------
#	Compiler options
#----------------------------

CC = gcc
CFLAGS = -Wall -O3 -g

COMMONFLAGS = -Isrc
DOSFLAGS = $(COMMONFLAGS)
XFLAGS 	= $(COMMONFLAGS)
DOSLIBS = -lseerd -lppcol -lalleg
XLIBS 	= -lseerd -lppcol -lalleg -L/usr/X11R6/lib -lXext -lX11 -lm


#----------------------------
#	Object dependencies
#----------------------------

COMMONOBJS = common.o report.o wrapper.o script.o tiles.o convtbl.o rpx.o \
		export.o exalleg.o player.o object.o weapon.o

MAPEDITOROBJS = $(COMMONOBJS) mapedit.o dirty.o maptiles.o mapobjs.o 
		
ENGINEOBJS = $(COMMONOBJS) engine.o

DOSMAPEDITOROBJS = $(addprefix dosobj/,$(MAPEDITOROBJS))
XMAPEDITOROBJS   = $(addprefix xobj/,$(MAPEDITOROBJS))

DOSENGINEOBJS = $(addprefix dosobj/,$(ENGINEOBJS))
XENGINEOBJS   = $(addprefix xobj/,$(ENGINEOBJS))

AUTOEXP_SRC = defs.h mapedit.h script.h tiles.h wrapper.h object.h \
	      player.h weapon.h
    
AUTOEXP_DEPS = $(addprefix src/,$(AUTOEXP_SRC))


#----------------------------
#	Targets
#----------------------------

.PHONY : gen clean xclean dosclean clearbackups dos X all

all : 
	@echo "Pick a target, any target:"
	@echo "	   make { dos X }"

dosobj/export.o : src/_export.c
xobj/export.o : src/_export.c

dosobj/%.o : src/%.c
	$(COMPILE.c) $(DOSFLAGS) -o $@ $<

xobj/%.o : src/%.c
	$(COMPILE.c) $(XFLAGS) -o $@ $<
	
mapedit.exe : $(DOSMAPEDITOROBJS)
	$(CC) $(DOSFLAGS) -o $@ $(DOSMAPEDITOROBJS) $(DOSLIBS)

mapedit : $(XMAPEDITOROBJS) 
	$(CC) $(XFLAGS) -o $@ $(XMAPEDITOROBJS) $(XLIBS)
	
engine.exe : $(DOSENGINEOBJS)
	$(CC) $(DOSFLAGS) -o $@ $(DOSENGINEOBJS) $(DOSENGINELIBS) $(DOSLIBS) 

engine : $(XENGINEOBJS) 
	$(CC) $(XFLAGS) -o $@ $(XENGINEOBJS) $(XENGINELIBS) $(XLIBS) 


#----------------------------
#	Auto-exports
#----------------------------

scripts/_imports.sh src/_export.c : $(AUTOEXP_DEPS) tools/autoexp.c
	./autoexp --output-seer-hdr=scripts/_imports.sh --output-c-src=src/_export.c $(AUTOEXP_DEPS)
	
gen : scripts/_imports.sh src/_export.c

autoexp autoexp.exe : tools/autoexp.c
	$(CC) $(CFLAGS) -o $@ $<
	

dos : autoexp.exe gen mapedit.exe engine.exe
X : autoexp gen mapedit engine


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

