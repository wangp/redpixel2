include options


OBJS = obj/guiapi.o obj/guiloop.o obj/guiwm.o obj/guidraw.o obj/jshelper.o 


all : example$(EXE)


obj/%.o : %.c
	$(COMPILE.c) -o $@ $<


example$(EXE) : $(OBJS) obj/example.o obj/colours.o 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) $(LIBM)


clean :
	rm -f obj/*.o example$(EXE)

