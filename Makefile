##
## Makefile for Red Pixel II
##

include config.linux

SRCDIRS := src src/store src/magic src/fastsqrt src/jpgalleg \
	   src/gui src/ug src/editor

CC := gcc
CFLAGS := $(PLAT_TARGET) $(PLAT_CFLAGS) -Wall -D_REENTRANT \
	  -I libnet/include -I lua-4.1-work3/include \
	  $(addprefix -I,$(SRCDIRS)) -g
LDLIBS := $(PLAT_LIBS)
LDFLAGS := $(PLAT_LDFLAGS)

ifndef PROFILE
CFLAGS += -O2 -fomit-frame-pointer -funroll-loops
else
CFLAGS += -O2 -funroll-loops -march=pentium -pg
LDFLAGS := -pg
endif

PROGRAM := program$(PLAT_EXE)
OBJDIR := $(PLAT_OBJDIR)

#----------------------------------------------------------------------

MODULES_STORE :=				\
	hash					\
	store

MODULES_MAGIC :=				\
	magic4x4				\
	magichf					\
	magicrot

MODULES_FASTSQRT :=				\
	fastsqrt

MODULES_JPGALLEG :=				\
	jpeg

MODULES_GUI :=					\
	gui					\
	guiaccel				\
	guidirty				\
	guimouse				\
	guiwm					\
	guiwmdef

MODULES_UG :=					\
	ug					\
	ugblank					\
	ugbutton				\
	ugdialog				\
	ugevent					\
	uglayout				\
	ugmenu					\
	ugscroll				\
	ugtheme					\
	ugthmmot				\
	ugthmpaw				\
	ugwidget

MODULES_EDITOR :=				\
	cursor					\
	editarea				\
	editor					\
	edselect				\
	menu					\
	mdlights				\
	mdobject				\
	mdtiles					\
	mdstarts				\
	modemgr					\
	selbar

MODULES_GAME :=					\
	alloc					\
	bindings				\
	bitmask					\
	bitmaskg				\
	bitmaskr				\
	blood					\
	camera					\
	error					\
	extdata					\
	fps					\
	gameinit				\
	gameclt					\
	gamesrv					\
	getoptc					\
	loaddata				\
	main					\
	map					\
	mapfile					\
	messages				\
	mylua					\
	object					\
	objtypes				\
	packet					\
	path					\
	render					\
	sync					\
	textface				\
	timeout					\
	timeval					\
	yield

MODULES := 					\
	$(MODULES_STORE) 			\
	$(MODULES_MAGIC)			\
	$(MODULES_FASTSQRT)			\
	$(MODULES_JPGALLEG)			\
	$(MODULES_GUI)				\
	$(MODULES_UG)				\
	$(MODULES_EDITOR)			\
	$(MODULES_GAME)

OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODULES)))

#----------------------------------------------------------------------

all: $(PROGRAM)

vpath %.c $(SRCDIRS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

src/bindings.inc: src/bindgen.lua
	$(PLAT_LUABIN) $< > $@

src/objectet.inc: src/objgen.lua
	$(PLAT_LUABIN) $< > $@

$(PROGRAM): $(OBJS) $(PLAT_LIBNET) $(PLAT_LIBLUA)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

#----------------------------------------------------------------------

ifeq "$(PLATFORM)" "LINUX"

$(PLAT_LIBNET):
	cp libnet/makfiles/linux.mak libnet/port.mak
	$(MAKE) -C libnet lib

endif

## other OSes here

$(PLAT_LIBLUA):
	$(MAKE) -C lua-4.1-work3

#----------------------------------------------------------------------

SOURCES := $(addsuffix /*.c,$(SRCDIRS))

TAGS: $(SOURCES)
	etags $^

tags: $(SOURCES)
	ctags $^

#----------------------------------------------------------------------

doc/gui_api.html: src/gui/gui.h
	$(PLAT_LUABIN) -f tools/mtfm.lua $< > $@

doc/ug_api.html: src/ug/ug.h
	$(PLAT_LUABIN) -f tools/mtfm.lua $< > $@

mtfmdocs: doc/gui_api.html doc/ug_api.html

#----------------------------------------------------------------------

depend:
	gcc $(CFLAGS) -MM $(SOURCES) | sed 's,^\(.*[.]o:\),$$(OBJDIR)/\1,' > makefile.dep

-include makefile.dep

#----------------------------------------------------------------------

clean: 
	rm -f $(OBJS) core

cleaner: clean
	rm -f $(PROGRAM) 
	rm -f TAGS tags
	rm -f depend

#----------------------------------------------------------------------

EXCLUDE_LIST := *.o $(PROGRAM) TAGS tags depend
EXCLUDE := $(addprefix --exclude , $(EXCLUDE_LIST))

backup:
	cd ../ && tar zcvf `date +%Y%m%d`.tar.gz redstone $(EXCLUDE)

suidroot:
	chown root.games $(PROGRAM)
	chmod 4750 $(PROGRAM)


.PHONY: clean cleaner backup suidroot lua


##
## End of Makefile
##
