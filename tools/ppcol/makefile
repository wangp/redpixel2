############################################################																			 #
#					PPcol makefile, by Ivan	Baldo.              #
#	 Based on makefile from	Allegro by Shawn Hargreaves.    #
#				The 'clean'	and 'uninstall' targets            #
#			  require rm from	the fileutils package.          #
#		To	build the documentation	you need makedoc.exe      #
#		from the Allegro docs directory installed in the     #
#				  docs directory of this package.              #
#                                                          #
############################################################
# replace this definition if you are using PGCC!
# PGCC=1

OBJ = obj
LIB = lib/libppcol.a
LIBDEST = $(DJDIR)/lib/libppcol.a
INCDEST = $(DJDIR)/include/ppcol.h
DOCDEST = $(DJDIR)/info/ppcol.inf
DJP = $(DJDIR)/bin/djp.exe

ifeq ($(LFN),y)
HTML = html
else
HTML = htm
endif

WFLAGS = -Wall -W -Werror -Wno-unused

ifdef DEBUGMODE
# build a debug version
OFLAGS = -g3
LFLAGS =

else

ifdef PROFILEMODE
# build with profiling information
ifdef PGCC
OFLAGS = -pg -mpentium -O6 -ffast-math
else
OFLAGS = -pg -m486 -O3 -ffast-math
endif
LFLAGS = -pg

else

# build a normal optimised version
ifdef PGCC
OFLAGS = -mpentium -O6 -ffast-math -fomit-frame-pointer
else
OFLAGS = -m486 -O3 -ffast-math -fomit-frame-pointer
endif

ifdef SYMBOLMODE
# keep debug symbols
LFLAGS = 
else
# strip debug symbols
LFLAGS = -s

endif
endif
endif

CFLAGS = -I. -Isrc -I$(OBJ) $(WFLAGS) $(OFLAGS)
SFLAGS = -I. -Isrc -I$(OBJ) $(WFLAGS)

VPATH = docs examples tests src

PROGRAMS = examples tests

OBJS = ppcol.o ppmaskc.o ppamount.o ppmamoun.o

LIB_OBJS = $(addprefix $(OBJ)/, $(OBJS))

DOCS = ChangeLog.txt docs/changes.$(HTML) \
       ppcol.txt docs/ppcol.$(HTML) docs/ppcol.txi docs/ppcol.inf

.PRECIOUS: $(OBJ)/%.o

.PHONY: all msg lib install uninstall docs clean veryclean $(PROGRAMS)

all: msg $(LIB) $(PROGRAMS) docs install
	@echo All done.
	@echo To use PPCol, #include ppcol.h and link with liballeg.a and libppcol.a.
	@echo Example command line: gcc foobar.c -o foobar.exe -lalleg -lppcol
	@echo Enjoy!

msg:
	@echo Compiling PPCol. Please wait...

lib: $(LIB)

install: $(LIBDEST) $(INCDEST) $(DOCDEST)

docs: $(DOCS)

$(LIBDEST): $(LIB)
	copy lib\libppcol.a $(subst /,\,$(LIBDEST))

$(INCDEST): src/ppcol.h
	copy src\ppcol.h $(subst /,\,$(INCDEST))

$(DOCDEST): docs/ppcol.inf
ifneq ($(wildcard $(DJDIR)/bin/makeinfo.exe),)
	copy docs\ppcol.inf $(subst /,\,$(DOCDEST))
else
	@echo makeinfo not installed: skipping copy of ppcol.inf
endif

$(OBJ)/%.o: %.c ppcol.h
	gcc $(CFLAGS) -o $@ -c $<

*/%.exe: $(OBJ)/%.o $(LIB)
	gcc $(LFLAGS) -o $@ $< $(LIB) `allegro-config --libs`

docs/%.inf: docs/%.txi
ifneq ($(wildcard $(DJDIR)/bin/makeinfo.exe),)
	makeinfo --no-split -o $@ $<
else
	@echo makeinfo not installed: skipping generation of ppcol.inf
endif

docs/%.txi: docs/%._tx docs/makedoc.exe
	docs/makedoc.exe -texinfo $@ $<

docs/%.$(HTML): docs/%._tx docs/makedoc.exe
	docs/makedoc.exe -$(HTML) $@ $<

%.txt: docs/%._tx docs/makedoc.exe
	docs/makedoc.exe -ascii $@ $<

ChangeLog.txt: docs/changes._tx docs/makedoc.exe
	docs/makedoc.exe -ascii ChangeLog.txt docs/changes._tx

$(LIB): $(LIB_OBJS)
	ar rs $(LIB) $(LIB_OBJS)

clean:
	-rm -v obj/*.* lib/*.* docs/*.$(HTML) docs/*.txi docs/*.inf

veryclean: clean
	-rm -v ChangeLog \
	examples/*.exe

uninstall:
	-rm $(LIBDEST)
	-rm $(INCDEST)
	-rm $(DOCDEST)
	@echo All gone!

examples: examples/ex1.exe examples/ex2.exe examples/ex3.exe
tests: tests/bmark.exe

INTERNAL_DEPS = ppcol.o ppmaskc.o ppamount.o ppmamoun.o

$(addprefix $(OBJ)/, $(INTERNAL_DEPS)): $(INTERNAL_H)

