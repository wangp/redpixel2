##
## Makefile for Red Pixel II
##

SRCDIRS = src src/net src/store src/magic src/gui src/ug src/editor 

CFLAGS = -Wall $(addprefix -I,$(SRCDIRS)) -g
LOADLIBES = `allegro-config --libs` -llua -lnet

PROGRAM = program
OBJDIR = obj/linux

#----------------------------------------------------------------------

MODULES_STORE =					\
	hash					\
	store

MODULES_MAGIC =					\
	magic4x4				\
	magicrot

MODULES_GUI =					\
	gui					\
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
	newfont					\
	selbar

MODULES_NET =					\
	netclnt					\
	netmain					\
	netserv

MODULES_GAME =					\
	alloc					\
	bdobject				\
	bdstore					\
	bindings				\
	bitmask					\
	extdata					\
	fps					\
	game					\
	gameinit				\
	gameloop				\
	gamenet					\
	loaddata				\
	loadhelp				\
	luastack				\
	main					\
	map					\
	mapfile					\
	object					\
	objanim					\
	objlayer				\
	objtypes				\
	path					\
	render					\
	scripts					\
	yield

MODULES = 					\
	$(MODULES_STORE) 			\
	$(MODULES_MAGIC)			\
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

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(LOADLIBES)

#----------------------------------------------------------------------

SOURCES = $(addsuffix /*.c,$(SRCDIRS))

TAGS: $(SOURCES)
	etags $^

tags: $(SOURCES)
	gctags $^

#----------------------------------------------------------------------

depend:
	gcc $(CFLAGS) -MM $(SOURCES) | sed 's,^\(.*[.]o:\),$(OBJDIR)/\1,' > depend

-include depend

#----------------------------------------------------------------------

clean: 
	rm -f $(OBJS) core

cleaner: clean
	rm -f $(PROGRAM) 
	rm -f TAGS tags
	rm -f depend

#----------------------------------------------------------------------

backup:
	cd ../ && tar zcvf `date +%Y%m%d`.tar.gz redstone

suidroot:
	chown root.allegro $(PROGRAM)
	chmod 4750 $(PROGRAM)


.PHONY: clean cleaner backup suidroot 


##
## End of Makefile
##