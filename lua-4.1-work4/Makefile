# makefile for Lua hierarchy
# see INSTALL for installation instructions
# see config for customization instructions

LUA= .

include $(LUA)/config

# primary targets ("co" and "klean" are used for making the distribution)
all clean co klean:
	cd include; $(MAKE) $@
	cd src; $(MAKE) $@
	cd src/lib; $(MAKE) $@
	cd src/luac; $(MAKE) $@
	cd src/lua; $(MAKE) $@

# simple test to see Lua working
test:	all
	bin/lua test/hello.lua

# remove debug information from binaries
strip:
	$(STRIP) bin/lua bin/luac

# official installation
install: all strip
	mkdir -p $(INSTALL_BIN) $(INSTALL_INC) $(INSTALL_LIB) $(INSTALL_MAN)
	$(INSTALL_EXEC) bin/* $(INSTALL_BIN)
	$(INSTALL_DATA) include/*.h $(INSTALL_INC)
	$(INSTALL_DATA) lib/*.a $(INSTALL_LIB)
	$(INSTALL_DATA) doc/*.1 $(INSTALL_MAN)

# shared libraries (for Linux)
so:
	ld -o lib/liblua.so.$V -shared src/*.o
	ld -o lib/liblualib.so.$V -shared src/lib/*.o
	cd lib; ln -s liblua.so.$V liblua.so; ln -s liblualib.so.$V liblualib.so

# binaries using shared libraries
sobin:
	rm -f bin/lua bin/luac
	cd src/lua; $(MAKE)
	cd src/luac; $(MAKE)

# install shared libraries
soinstall:
	$(INSTALL_EXEC) lib/*.so.* $(INSTALL_LIB)
	cd $(INSTALL_LIB); ln -s liblua.so.$V liblua.so; ln -s liblualib.so.$V liblualib.so

# (end of Makefile)
