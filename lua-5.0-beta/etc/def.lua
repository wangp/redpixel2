-- def.lua
-- make .DEF file from lua.h
-- usage: lua def.lua <lua.h >lua.def

T=io.read"*a"
io.write("LIBRARY LUA\nVERSION ")
string.gsub(T,"LUA_VERSION.-(%d+%.%d+)",io.write)
io.write("\nEXPORTS\n")
string.gsub(T,"(luaL?_[%w%_]+)%s+%(",function (f) io.write("   ",f,"\n") end)
