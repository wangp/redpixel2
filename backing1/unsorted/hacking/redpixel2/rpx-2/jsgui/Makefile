include options

GUIOBJS = obj/guiapi.o obj/guiloop.o obj/guiwm.o \
	  obj/guidraw.o obj/guitimer.o obj/jshelper.o
	  
LIBGUI = obj/libgui.a

all: libjsgui.a example$(EXE)

obj/%.o: %.c
	$(COMPILE.c) -o $@ $<

$(LIBGUI): $(GUIOBJS)
	ar rs $@ $^

example$(EXE): obj/example.o obj/colours.o $(LIBGUI)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) $(LIBM)

libjsgui.a: $(GUIOBJS)
	ar rs $@ $^

clean:
	rm -f obj/*.o example$(EXE) core libjsgui.a
