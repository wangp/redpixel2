-- stdcall.lua
-- add __stdcall where appropriate
-- usage: lua stdcall.lua <lua.h >s_lua.h
-- usage: lua stdcall.lua <lapi.c >s_lapi.c

T=io.read"*a"
T=string.gsub(T,"LUA_API (lua_%w+%s+%()","__stdcall %1")
T=string.gsub(T,"(%*lua_CFunction)","__stdcall %1")

io.write(T)
