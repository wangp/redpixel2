##
## Makefile for Red Pixel II
##

SRCDIRS = src src/store src/gui src/ug src/editor

CFLAGS = -Wall $(addprefix -I,$(SRCDIRS)) -g
LOADLIBES = `allegro-config --libs` -llua

PROGRAM = main
OBJDIR = obj/linux

#----------------------------------------------------------------------

MODULES_STORE =					\
	hash					\
	store

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
	modemgr					\
	newfont					\
	selbar

MODULES_GAME =					\
	alloc					\
	bdobject				\
	bdstore					\
	bindings				\
	fps					\
	game					\
	gameloop				\
	loaddata				\
	loadhelp				\
	luastack				\
	magic4x4				\
	magicld					\
	magicrot				\
	map					\
	mapfile					\
	objlayer				\
	objtypes				\
	path					\
	render					\
	scripts

MODULES = 					\
	$(MODULES_STORE) 			\
	$(MODULES_GUI)				\
	$(MODULES_UG)				\
	$(MODULES_EDITOR)			\
	$(MODULES_GAME)

OBJS = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODULES)))

#----------------------------------------------------------------------

vpath %.c $(SRCDIRS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(PROGRAM): $(OBJS)

#----------------------------------------------------------------------

SOURCES = $(addsuffix /*.c,$(SRCDIRS))

TAGS: $(SOURCES)
	etags $^

tags: $(SOURCES)
	gctags $^

#----------------------------------------------------------------------

.PHONY = backup suidroot clean cleaner

backup:
	cd ../ && tar zcvf `date +%Y%m%d`.tar.gz redstone

suidroot:
	chown root.allegro $(PROGRAM)
	chmod 4750 $(PROGRAM)

clean: 
	rm -f $(OBJS) core

cleaner: clean
	rm -f $(PROGRAM) 
	rm -f TAGS tags


##
## End of Makefile
##