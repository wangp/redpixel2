-- objgen.lua -- generate game object field accessors


-- supported types

Float = 'Float'
Int = 'Int'
Bool = 'Bool'

types = {
    Float = { 
	predicate = 'lua_isnumber',
	get = 'lua_pushnumber(L, @VAR@);',
	set = '@VAR@ = lua_tonumber(@LUA@);'
    },
    Int = {
	predicate = 'lua_isnumber',
	get = 'lua_pushnumber(L, @VAR@);',
	set = '@VAR@ = lua_tonumber(@LUA@);'
    },
    Bool = {
	predicate = nil,
	get = 'if (@VAR@) lua_pushnumber(L, @VAR@); else lua_pushnil(L);',
	set = '@VAR@ = !lua_isnil(@LUA@);'
    }
}


-- variables from struct object that should be read/write from Lua

cvars = {{ Float, 'x' },
	 { Float, 'y' },
	 { Float, 'xv' },
	 { Float, 'yv' },
	 { Float, 'mass' },
	 { Int, 'ramp' },
	 { Bool, "is_proxy" }}


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
    p (2, gsub(vtype.get, '@VAR@', 'quux->'..vname))
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
    if vtype.predicate then
	p (2, 'if (!'..vtype.predicate..'(L, 3)) lua_error(L, "type mismatch in assignment");')
    end
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


-- tag method registration
print ([[
#define REGISTER_OBJECT_TAG_METHODS(L,TAG)						\
	lua_pushcfunction(L, object_gettable_tm); lua_settagmethod(L, TAG, "gettable"); \
	lua_pushcfunction(L, object_settable_tm); lua_settagmethod(L, TAG, "settable"); \
]])


-- objgen.lua ends here
