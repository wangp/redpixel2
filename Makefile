##
## Makefile for Red Pixel II
##

SRCDIRS = src src/net src/store src/magic src/fastsqrt \
	  src/gui src/ug src/editor

CC = gcc
CFLAGS = -Wall $(addprefix -I,$(SRCDIRS)) -g
CFLAGS += -O2 -fomit-frame-pointer -funroll-loops -mpentium
LOADLIBES = `allegro-config --libs` -llua -llualib -lnet -lpthread

PROGRAM = program
OBJDIR = obj/linux

#----------------------------------------------------------------------

MODULES_STORE =					\
	hash					\
	store

MODULES_MAGIC =					\
	magic4x4				\
	magicrot				\
	magicrt

MODULES_FASTSQRT =				\
	fastsqrt

MODULES_GUI =					\
	gui					\
	guiaccel				\
	guidirty				\
	guimouse				\
	guiwm					\
	guiwmdef

MODULES_UG =					\
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

MODULES_EDITOR =				\
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

MODULES_NET =					\
	netclnt					\
	netmain					\
	netserv

MODULES_GAME =					\
	alloc					\
	bindings				\
	bitmask					\
	bitmaskg				\
	bitmaskr				\
	camera					\
	error					\
	extdata					\
	fps					\
	game					\
	gameinit				\
	loaddata				\
	main					\
	map					\
	mapfile					\
	mylua					\
	object					\
	objtypes				\
	path					\
	render					\
	yield

MODULES = 					\
	$(MODULES_STORE) 			\
	$(MODULES_MAGIC)			\
	$(MODULES_FASTSQRT)			\
	$(MODULES_GUI)				\
	$(MODULES_UG)				\
	$(MODULES_EDITOR)			\
	$(MODULES_NET)				\
	$(MODULES_GAME)

OBJS = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODULES)))

#----------------------------------------------------------------------

all: $(PROGRAM)

vpath %.c $(SRCDIRS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

src/bindings.inc: src/bindgen.lua
	lua $< > $@

src/objecttm.inc: src/objgen.lua
	lua $< > $@

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(LOADLIBES)

#----------------------------------------------------------------------

SOURCES = $(addsuffix /*.c,$(SRCDIRS))

TAGS: $(SOURCES)
	etags $^

tags: $(SOURCES)
	gctags $^

#----------------------------------------------------------------------

doc/gui_api.html: src/gui/gui.h
	lua -f tools/mtfm.lua $< > $@

doc/ug_api.html: src/ug/ug.h
	lua -f tools/mtfm.lua $< > $@

mtfmdocs: doc/gui_api.html doc/ug_api.html

#----------------------------------------------------------------------

depend:
	gcc $(CFLAGS) -MM $(SOURCES) | sed 's,^\(.*[.]o:\),$(OBJDIR)/\1,' > makefile.dep

-include makefile.dep

#----------------------------------------------------------------------

clean: 
	rm -f $(OBJS) core

cleaner: clean
	rm -f $(PROGRAM) 
	rm -f TAGS tags
	rm -f depend

#----------------------------------------------------------------------

EXCLUDE_LIST = *.o $(PROGRAM) TAGS tags depend
EXCLUDE = $(addprefix --exclude , $(EXCLUDE_LIST))

backup:
	cd ../ && tar zcvf `date +%Y%m%d`.tar.gz redstone $(EXCLUDE)

suidroot:
	chown root.games $(PROGRAM)
	chmod 4750 $(PROGRAM)


.PHONY: clean cleaner backup suidroot


##
## End of Makefile
##
