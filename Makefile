##
## Makefile for Red Pixel II
##

ifdef MINGDIR
	include config.mingw
else
	include config.linux
endif

SRCDIRS := src src/store src/magic src/fastsqrt src/jpgalleg \
	   src/loadaud src/gui src/ug src/editor src/server

CC := gcc
CFLAGS := $(PLAT_TARGET) $(PLAT_CFLAGS) -Wall -D_REENTRANT \
	  -I libnet/include -I lua-4.1-work4/include \
	  $(addprefix -I,$(SRCDIRS)) -g -Wstrict-prototypes
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

MODULES_LOADAUD :=				\
	loadaud

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

MODULES_SERVER :=				\
	clsvface				\
	server					\
	svclient				\
	svgame					\
	svlobby					\
	svstats					\
	svticker				\
	textface				\

MODULES_GAME :=					\
	alloc					\
	bindings				\
	bitmask					\
	bitmaskg				\
	bitmaskr				\
	blast					\
	blod					\
	camera					\
	client					\
	error					\
	explo					\
	extdata					\
	fps					\
	gameinit				\
	gamemenu				\
	getoptc					\
	loaddata				\
	main					\
	map					\
	mapfile					\
	messages				\
	mylibnet				\
	mylua					\
	object					\
	objtypes				\
	packet					\
	particle				\
	path					\
	render					\
	sound					\
	sync					\
	timeout					\
	timeval					\
	yield

MODULES := 					\
	$(MODULES_STORE) 			\
	$(MODULES_MAGIC)			\
	$(MODULES_FASTSQRT)			\
	$(MODULES_JPGALLEG)			\
	$(MODULES_LOADAUD)			\
	$(MODULES_GUI)				\
	$(MODULES_UG)				\
	$(MODULES_EDITOR)			\
	$(MODULES_SERVER)			\
	$(MODULES_GAME)

OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODULES)))

#----------------------------------------------------------------------

all: $(PROGRAM)

vpath %.c $(SRCDIRS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

src/bindings.inc: src/bindgen.lua
	$(PLAT_LUABIN) $< > $@

src/objectmt.inc: src/objgen.lua
	$(PLAT_LUABIN) $< > $@

$(PROGRAM): $(OBJS) $(PLAT_LIBNET) $(PLAT_LIBLUA)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

#----------------------------------------------------------------------

ifeq "$(PLATFORM)" "LINUX"

$(PLAT_LIBNET):
	cp libnet/makfiles/linux.mak libnet/port.mak
	$(MAKE) -C libnet lib

$(PLAT_LIBLUA):
	$(MAKE) -C lua-4.1-work4

endif

ifeq "$(PLATFORM)" "MINGW"

$(PLAT_LIBNET):
	copy libnet\makfiles\mingw.mak libnet\port.mak
	$(MAKE) -C libnet lib

$(PLAT_LIBLUA):
	$(MAKE) -C lua-4.1-work4/src
	$(MAKE) -C lua-4.1-work4/src/lib
	$(MAKE) -C lua-4.1-work4/src/lua
endif

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

makefile.dep: $(SOURCES)
	gcc $(CFLAGS) -MM $(SOURCES) | sed 's,^\(.*[.]o:\),$$(OBJDIR)/\1,' > makefile.dep

depend: makefile.dep

include makefile.dep

#----------------------------------------------------------------------

clean: 
	rm -f $(OBJS) core

cleaner: clean
	rm -f $(PROGRAM) 
	rm -f TAGS tags
	rm -f makefile.dep

#----------------------------------------------------------------------

EXCLUDE_LIST := *.o $(PROGRAM) TAGS tags depend
EXCLUDE := $(addprefix --exclude , $(EXCLUDE_LIST))

backup:
	cd ../ && tar zcvf redstone-`date +%Y%m%d`.tar.gz redstone $(EXCLUDE)

suidroot:
	chown root.games $(PROGRAM)
	chmod 4750 $(PROGRAM)

prepare-dist: cleaner
	$(MAKE) depend
	$(MAKE) -C libnet cleaner
	rm libnet/port.mak
	$(MAKE) -C lua-4.1-work4 clean

.PHONY: clean cleaner backup suidroot prepare-dist


##
## End of Makefile
##
