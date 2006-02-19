##
## Makefile for Red Pixel II
##

ifdef MINGDIR
	include config.mingw
else
	include config.linux
endif

SRCDIRS := src src/store src/magic src/fastsqrt \
	   src/loadaud src/2xsai src/gui src/ug src/editor src/server

CC := gcc
CFLAGS := $(PLAT_TARGET) $(PLAT_CFLAGS) -D_REENTRANT \
	  -W -Wall -Wno-unused-parameter -Wno-deprecated-declarations \
	  -I libnet/include -I lua/include -I dumb/include \
	  -I jpgalleg/include \
	  $(addprefix -I,$(SRCDIRS)) -Wstrict-prototypes -pipe
LDLIBS := $(PLAT_LIBS)
LDFLAGS := $(PLAT_LDFLAGS)

ifndef PROFILE
ifndef DEBUG
CFLAGS += -O3 -fomit-frame-pointer -funroll-loops -ffast-math
else
CFLAGS += -O3 -g
endif
else
CFLAGS += -O3 -funroll-loops -pg
LDFLAGS := -pg
endif

PROGRAM := program$(PLAT_EXE)
OBJDIR := $(PLAT_OBJDIR)

#----------------------------------------------------------------------

MODULES_STORE :=				\
	store

MODULES_MAGIC :=				\
	magic4x4				\
	magichf					\
	magicrot

MODULES_FASTSQRT :=				\
	fastsqrt

MODULES_LOADAUD :=				\
	loadaud

MODULES_2XSAI :=				\
	2xsai

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
	fe-credits				\
	fe-lobby				\
	fe-main					\
	fe-options				\
	fe-widgets				\
	fps					\
	gameinit				\
	gamma					\
	getoptc					\
	loaddata				\
	main					\
	map					\
	mapfile					\
	messages				\
	music					\
	mylibnet				\
	mylua					\
	object					\
	objtypes				\
	packet					\
	particle				\
	path					\
	ral					\
	render					\
	screen					\
	set-video				\
	sound					\
	strlcat					\
	strlcpy					\
	sync					\
	textout					\
	timeout					\
	timeval					\
	yield

MODULES := 					\
	$(MODULES_STORE) 			\
	$(MODULES_MAGIC)			\
	$(MODULES_FASTSQRT)			\
	$(MODULES_LOADAUD)			\
	$(MODULES_2XSAI)			\
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

$(OBJDIR)/fastsqrt.o: fastsqrt.c
	$(CC) $(CFLAGS) -fno-strict-aliasing -o $@ -c $<

src/bindings.inc: src/bindgen.lua
	$(PLAT_LUABIN) $< > $@

src/objectmt.inc: src/objgen.lua
	$(PLAT_LUABIN) $< > $@

$(PROGRAM): $(OBJS) $(PLAT_LIBNET) $(PLAT_LIBLUA) $(PLAT_LIBDUMB) $(PLAT_LIBALDUMB)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

#----------------------------------------------------------------------

ifeq "$(PLATFORM)" "LINUX"

$(PLAT_LIBNET):
	cp libnet/makfiles/linux.mak libnet/port.mak
	$(MAKE) -C libnet lib

$(PLAT_LIBLUA):
	$(MAKE) -C lua

$(PLAT_LIBDUMB):
	cd dumb ; ./fix.sh unix
	$(MAKE) -C dumb core

$(PLAT_LIBALDUMB):
	cd dumb ; ./fix.sh unix
	$(MAKE) -C dumb lib/unix/libaldmb.a

$(PLAT_LIBJPGALLEG):
	cd jpgalleg ; ./fix.sh unix --quick
	$(MAKE) -C jpgalleg lib STATIC=1

endif

ifeq "$(PLATFORM)" "MINGW"

$(PLAT_LIBNET):
	$(MAKE) -C libnet lib

$(PLAT_LIBLUA):
	$(MAKE) -C lua/src
	$(MAKE) -C lua/src/lib
	$(MAKE) -C lua/src/lua

$(PLAT_LIBDUMB):
	$(MAKE) -C dumb core

$(PLAT_LIBALDUMB):
	$(MAKE) -C dumb lib/mingw/libaldmb.a

$(PLAT_LIBJPGALLEG):
	$(MAKE) -C jpgalleg lib STATIC=1

endif

#----------------------------------------------------------------------

SOURCES := $(addsuffix /*.c,$(SRCDIRS))

TAGS: $(SOURCES)
	etags $^

# TAGS == tags on Windows :-P
ifneq "$(PLATFORM)" "MINGW"

tags: $(SOURCES)
	ctags $^

endif

#----------------------------------------------------------------------

doc/gui_api.html: src/gui/gui.h
	$(PLAT_LUABIN) tools/mtfm.lua $< > $@

doc/ug_api.html: src/ug/ug.h
	$(PLAT_LUABIN) tools/mtfm.lua $< > $@

mtfmdocs: doc/gui_api.html doc/ug_api.html

#----------------------------------------------------------------------

depend: $(SOURCES)
	gcc $(CFLAGS) -MM $(SOURCES) | sed 's,^\(.*[.]o:\),$$(OBJDIR)/\1,' > makefile.dep

-include makefile.dep

#----------------------------------------------------------------------

clean: 
	rm -f $(OBJS) core

cleaner: clean
	rm -f $(PROGRAM) 
	rm -f TAGS tags
	rm -f makefile.dep

#----------------------------------------------------------------------

.PHONY: ChangeLog
ChangeLog:
	( echo '	This ChangeLog is automatically produced from CVS logs.'; \
	  echo '	See also ChangeOld and ChangeOlder files.'; echo; \
	  tools/cvs2cl.pl --stdout ) > ChangeLog

EXCLUDE_LIST := *.o $(PROGRAM) TAGS tags depend
EXCLUDE := $(addprefix --exclude , $(EXCLUDE_LIST))

backup:
	cd ../ && tar ycvf redstone-`date +%Y%m%d`.tar.bz2 redstone $(EXCLUDE)

suidroot:
	chown root.games $(PROGRAM)
	chmod 4750 $(PROGRAM)

prepare-dist: cleaner
	$(MAKE) depend
	$(MAKE) -C libnet cleaner
	rm libnet/port.mak
	$(MAKE) -C lua clean

.PHONY: clean cleaner backup suidroot prepare-dist


##
## End of Makefile
##
