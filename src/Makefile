DEBUG=1

ifdef DEBUG
	CFLAGS = -Wall -Igui -Iug -Ieditor -Istore -g
	LOADLIBES = `allegro-config --libs`
else
	CFLAGS = -Wall -O3
	LOADLIBES = -lstone `allegro-config --libs`
endif

ifdef DEBUG
	PROGRAM = main

	OBJS = 					\
	editor/alloc.o				\
	editor/editarea.o			\
	editor/editor.o				\
	editor/edselect.o			\
	editor/lights.o				\
	editor/magic4x4.o			\
	editor/magicld.o			\
	editor/map.o				\
	editor/modemgr.o			\
	editor/newfont.o			\
	editor/resource.o			\
	editor/selbar.o				\
	editor/tiles.o				\
	gui/gui.o				\
	gui/guidirty.o				\
	gui/guimouse.o				\
	gui/guiwm.o				\
	gui/guiwmdef.o				\
	ug/ug.o					\
	ug/ugblank.o				\
	ug/ugbutton.o				\
	ug/ugdialog.o				\
	ug/ugevent.o				\
	ug/uglayout.o				\
	ug/ugscroll.o				\
	ug/ugtheme.o				\
	ug/ugthmmot.o				\
	ug/ugthmpaw.o				\
	ug/ugwidget.o				\
	store/hash.o				\
	store/store.o
else
	STUB 	  = stub
	STUBOBJS  = stub.o export.o
endif

$(PROGRAM): $(OBJS)

$(STUB): $(STUBOBJS) 

#------------------------------------------------------------

SOURCES   = *.c *.h editor/*.c editor/*.h gui/*.c gui/*.h ug/*.c ug/*.h

TAGS: $(SOURCES)
	etags $^

tags: $(SOURCES)
	gctags $^

#------------------------------------------------------------

.PHONY = suidroot clean cleaner

suidroot:
	chown root.allegro $(STUB)
	chmod 4750 $(STUB)

clean: 
	rm -f $(STUBOBJS) $(OBJS)
	rm -f gui/*.o ug/*.o editor/*.o 
	rm -f gui/core ug/core editor/core

cleaner: clean
	rm -f $(STUB) $(PROGRAM)
	rm -f TAGS tags
