-- objgen.lua -- generate game object field accessors


-- supported types (currently only floats)

Float = 'Float'

types = {
    Float = { 
	predicate = 'lua_isnumber',
	get = 'lua_pushnumber',
	set = '@VAR@ = lua_tonumber(@LUA@);'
    }
}


-- variables from struct object that should be read/write from Lua

cvars = {{ Float, 'x' },
	 { Float, 'y' },
	 { Float, 'xv' },
	 { Float, 'yv' },
	 { Float, 'mass' },
	 { Float, 'ramp' }}


-- helpers
	 
function p (indent, string)
    for i = 1,indent do string = '\t'..string end
    print (string)
end


-- header

p (0, '/* Generated by objgen.lua on '..date()..'.  Do not edit.  */\n')


-- gettable tag method

p (0, 'static int')
p (0, 'object_gettable_tm(lua_State *L)')
p (0, '{')
p (1, 'struct object *quux = lua_touserdata(L, 1);')
p (1, 'const char *index = lua_tostring(L, 2);')
for i,v in cvars do
    local vtype, vname = types[v[1]], v[2]
    p (1, ((i == 1) and '' or 'else ')..'if (0 == strcmp(index, "'..vname..'")) {')
    p (2, vtype.get..'(L, quux->'..vname..');')
    p (1, '}')
end
p (1, 'else {')
p (2, 'lua_getref(L, quux->table);')
p (2, 'lua_pushvalue(L, 2);')
p (2, 'lua_rawget(L, -2);')
p (1, '}')
p (1, 'return 1;')
p (0, '}\n')


-- settable tag method

p (0, 'static int')
p (0, 'object_settable_tm(lua_State *L)')
p (0, '{')
p (1, 'struct object *quux = lua_touserdata(L, 1);')
p (1, 'const char *index = lua_tostring(L, 2);')
for i,v in cvars do
    local vtype, vname = types[v[1]], v[2]
    p (1, ((i == 1) and '' or 'else ')..'if (0 == strcmp(index, "'..vname..'")) {')
    p (2, 'if (!'..vtype.predicate..'(L, 3)) lua_error(L, "type mismatch in assignment");')
    p (2, gsub(gsub(vtype.set, '@VAR@', 'quux->'..vname), '@LUA@', 'L, 3'))
    p (1, '}')
end
p (1, 'else {')
p (1, '\tlua_getref(L, quux->table);')
p (1, '\tlua_pushvalue(L, 2);')
p (1, '\tlua_pushvalue(L, 3);')
p (1, '\tlua_rawset(L, -3);')
p (1, '}')
p (1, 'return 1;')
p (0, '}')


-- gc tag method (currently no types require gc)

print ([[
static int
object_gc_tm(lua_State *L)
{
	return 0;
}]])


-- tag method registration
print ([[
#define REGISTER_OBJECT_TAG_METHODS(L,TAG)						\
	lua_pushcfunction(L, object_gettable_tm); lua_settagmethod(L, TAG, "gettable"); \
	lua_pushcfunction(L, object_settable_tm); lua_settagmethod(L, TAG, "settable"); \
	lua_pushcfunction(L, object_gc_tm); lua_settagmethod(L, TAG, "gc");
]])


-- objgen.lua ends here
