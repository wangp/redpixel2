#
#  Makefile for teaching grandmothers...
#
#  By Shawn Hargreaves.


.PHONY: djgpp msvc linux clean backup zip

.PRECIOUS: %.o %.obj

GCC_FLAGS = -march=i686 -O3 -Wall -MMD -g

GCC_LDFLAGS = 

MSVC_FLAGS = -W3 -WX -Gd -Ox -GB -MT

GCC_OBJS = eval.o interp.o load.o render.o unload.o var.o

MSVC_OBJS = eval.obj interp.obj load.obj render.obj unload.obj var.obj

djgpp: libegg.a eggshell.exe evaltest.exe

msvc: egg.lib egg_win.exe eval_win.exe

linux: libegg.a eggshell evaltest

eggshell.exe: eggshell.o libegg.a
	gcc $(GCC_LDFLAGS) -o eggshell.exe eggshell.o libegg.a -lalleg

evaltest.exe: evaltest.o libegg.a
	gcc $(GCC_LDFLAGS) -o evaltest.exe evaltest.o libegg.a -lalleg

egg_win.exe: eggshell.obj egg.lib
	cl -nologo -Feegg_win.exe eggshell.obj egg.lib alleg.lib

eval_win.exe: evaltest.obj egg.lib
	cl -nologo -Feeval_win.exe evaltest.obj egg.lib alleg.lib

eggshell: eggshell.o libegg.a
	gcc $(GCC_LDFLAGS) -o eggshell eggshell.o libegg.a `allegro-config --libs`

evaltest: evaltest.o libegg.a
	gcc $(GCC_LDFLAGS) -o evaltest evaltest.o libegg.a `allegro-config --libs`

libegg.a: $(GCC_OBJS)
	ar rs libegg.a $(GCC_OBJS)

egg.lib: $(MSVC_OBJS)
	lib -nologo -out:egg.lib $(MSVC_OBJS)

clean:
	rm -f *.o *.obj *.d *.a *.lib *.exe *.mft *.zip eggshell evaltest

backup:
	copy *.c backup
	copy *.h backup
	copy makefile backup
	copy examples\*.egg backup

zip:
	rm -f egg.zip
	zip -9 egg.zip *.txt *.c *.h makefile examples/*.egg
	echo egg.mft > egg.mft
	unzip -Z1 egg.zip >> egg.mft
	zip -9 egg.zip egg.mft

%.o: %.c
	gcc $(GCC_FLAGS) -o $@ -c $<

%.obj: %.c
	cl -nologo $(MSVC_FLAGS) -Fo$@ -c $<

-include *.d
