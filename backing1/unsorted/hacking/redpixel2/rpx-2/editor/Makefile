basedir = ..
include $(basedir)/options


RPXEDIT_OBJ_LIST = stub.o native.o draw.o basehelp.o import.o jshelper.o \
 		colours.o datalist.o xmlbind.o
RPXEDIT_OBJS = $(addprefix $(objdir)/,$(RPXEDIT_OBJ_LIST))


all: $(basedir)/rpxedit$(EXE)


$(basedir)/rpxedit$(EXE): $(RPXEDIT_OBJS) 
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
