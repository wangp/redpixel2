-- bindgen.lua -- generate Lua <-> C bindings

gsub = string.gsub
getn = table.getn
date = os.date


-- Types that we should know about.
--	field 1: checkargs type char
--	field 2: C type
--	field 3: Lua to C conversion function
--	field 4: optional error check; $ is replaced by the variable name

ctypes = {
    -- builtin types
    Float	= { "n", "float", "lua_tonumber" },
    Function 	= { "f", "lua_ref_t", "lua_ref" },
    Int	 	= { "n", "int", "lua_tonumber" },
    Bool	= { "b", "int", "lua_toboolean" },
    String	= { "s", "const char *", "lua_tostring" },
    -- custom and readability types
    Method   	= { "f", "lua_ref_t", "lua_ref" },
    Object	= { "u", "object_t *", "lua_toobject", "!$" },
    ObjId	= { "n", "objid_t", "lua_tonumber" },
    ObjTag	= { "n", "objtag_t", "lua_tonumber" },
    ClientId	= { "n", "int", "lua_tonumber" },
    StoreKey	= { "s", "const char *", "lua_tostring" }
}

-- convenience
Float = "Float"
Function = "Function"
Int = "Int"
Bool = "Bool"
String = "String"
Method = "Method"
Object = "Object"
ObjId = "ObjId"
ObjTag = "ObjTag"
ClientId = "ClientId"
StoreKey = "StoreKey"


-- Code generator.
--	lname: 	 Lua function name; if omitted is the same as cname
--	cname: 	 C function name
--	check:	 argument checking string; if omitted will be generated
--	args: 	 table of arguments of the form:
--		     { type, variable name, optional error check, optional code snippet }
--	      	   If the error check is specified, it is used in preference
--		     to the general error check in the type table.
--		   If the code snippet is specified, it is used in preference
--		     to the default generated conversion code.  In the snippet,
--		     $# is replaced by the argument number, and $ is replaced
--		     by the variable name
--    	ret: 	 return value, in same format as args
--	success: code to execute on success; default is to return 1
--	error:	 code to execute on error; default is to return nil

function generate_code (lname, cname, check, args, ret, success, error)
    lname = lname or cname
    local tab = "\t"
    local str

    function generate_check ()
	local str = ""
	for i,v in args do
	    local typ = ctypes[v[1]]
	    str = str..typ[1]
	end
	return str
    end

    -- function declaration
    str = "static int bind_"..lname.."(lua_State *L)\n{\n"

    -- argument declarations
    for i,v in args do
	local typ = ctypes[v[1]]
	str = str..tab..typ[2].." "..v[2]..";\n"
    end

    if ret then
	local typ = ctypes[ret[1]]
	str = str..tab..typ[2].." "..ret[2]..";\n"
    end

    -- argument checking
    str = str..tab..'if (!lua_checkargs(L, "'..(check or generate_check())..'")) goto badargs;\n'

    -- argument conversion
    for i,v in args do
	local typ = ctypes[v[1]]
	if v[4] then
	    local x = gsub (gsub (v[4], "%$#", i), "%$", v[2])
	    str = str..tab..x.."\n"
	else
	    str = str..tab..v[2].." = "..typ[3].."(L, "..i..");\n"
	end
	if v[3] then 
	    str = str..tab.."if ("..v[3]..") goto badargs;\n"
	elseif typ[4] then
	    str = str..tab.."if ("..gsub(typ[4], "%$", v[2])..") goto badargs;\n"
	end
    end

    -- function call
    str = str..tab
    if ret and ret[4] then
	local x = gsub (ret[4], "%$", ret[2])
	str = str..x.."\n"
    else
	if ret then str = str..ret[2].." = " end
	str = str..cname.."("
	for i,v in args do
	    if i == getn (args) then
		str = str..v[2]..""
	    else
		str = str..v[2]..", "
	    end
	end
	str = str..");\n"
    end

    -- check return
    if ret and ret[3] then
	str = str..tab.."if ("..gsub(ret[3], "%$", ret[2])..") goto error;\n"
    end

    -- success clause
    str = str..tab..(success or "lua_pushboolean(L, 1); return 1;").."\n"

    -- bad args clause
    str = str..
	"badargs:\n"..tab..'printf("bad args to '..lname..'\\n");\n'..
	tab.."goto error; /* shut up the compiler about unused labels */\n"

    -- error clause
    str = str.."error:\n"..tab..(error or "lua_pushboolean(L, 0); return 1;").."\n"

    str = str.."}\n"
    print (str)
end

reg_init = "#define DO_REGISTRATION_INIT(L) "
unreg_init = "#define DO_UNREGISTRATION_INIT(L) "
reg_both = "#define DO_REGISTRATION_BOTH(L) "
reg_server = "#define DO_REGISTRATION_SERVER(L) "
reg_client = "#define DO_REGISTRATION_CLIENT(L) "

function _generate (table, reglist)
    generate_code (table.lname, table.cname, table.check, table.args, 
		   table.ret, table.success, table.error)

    return reglist..
	' \\\n\tlua_register(L, "'..(table.lname or table.cname)..
	'", bind_'..(table.lname or table.cname)..');'
end

function generate_init (table)	-- expose only at init time
    reg_init = _generate (table, reg_init)
    unreg_init = unreg_init..' \\\n\t'..
	'lua_pushnil(L); '..
	'lua_setglobal(L, "'..(table.lname or table.cname)..'");'
end

function generate_both (table)	-- expose to both namespaces
    reg_both = _generate (table, reg_both)
end

function generate_server (table) -- expose to server namespace only
    reg_server = _generate (table, reg_server)
end

function generate_client (table) -- expose to client namespace only
    reg_client = _generate (table, reg_client)
end


-- Begin output.

print ("/* Generated by bindgen.lua on "..date()..".  Do not edit.  */\n")


-------------
-- store.h --
-------------

generate_init {
    lname	= "store_load",
    args	= {{ String, "filename" },
		   { String, "prefix" }},
    ret		= { Int, "ret", "$",
		    "$ = !store_load_ex(filename, prefix, load_extended_datafile);" }
}


----------------
-- loaddata.h --
----------------

generate_init {
    cname	= "tiles_load",
    args	= {{ String, "filename" },
                   { String, "prefix" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_init {
    cname	= "lights_load",
    args	= {{ String, "filename" },
                   { String, "prefix" }},
    ret		= { Int, "ret", "$ < 0" }
}


----------------
-- objtypes.h --
----------------

generate_init {
    lname	= "objtype_register",
    cname	= "objtypes_register",
    check	= "[sN]ss[fN-]",
    args	= {{ String, "type", nil,
	             "$ = lua_isnil(L, $#) ? 0 : lua_tostring(L, $#);" },
    		   { String, "name" },
		   { String, "icon" },
		   { Method, "func", nil, 
		     "$ = (lua_isnoneornil(L, $#) ? LUA_NOREF : lua_ref(L, $#));" }},
    ret		= { Int, "ret", "$ < 0" }
}


-------------
-- explo.h --
-------------

generate_init {
    lname	= "explosion_type_register",
    cname	= "explosion_type_register",
    check	= "ssnn[sN-][sN-]",
    args	= {{ String, "name" },
		   { String, "first_frame" },
		   { Int, "nframes" },
		   { Int, "tics" },
	       	   { String, "light", nil,
		     "$ = (lua_isnoneornil(L, $#) ? 0 : lua_tostring(L, $#));" },
	           { String, "sound", nil,
		     "$ = (lua_isnoneornil(L, $#) ? 0 : lua_tostring(L, $#)); "}},
    ret		= { Int, "ret", "$ < 0" }
}


--------------
-- object.h --
--------------

generate_init {
    cname	= "new_object_collision_tag",
    args	= {},
    ret		= { ObjTag, "ret" },
    success	= "lua_pushnumber(L, ret); return 1;"
}

generate_server {
    cname	= "object_set_stale",
    lname	= "object_set_stale",
    args	= {{ Object, "obj" }}
}

generate_server {
    cname	= "object_hide",
    args	= {{ Object, "obj" }}
}

generate_server {
    cname	= "object_show",
    args	= {{ Object, "obj" }}
}

generate_client {
    cname	= "object_set_highlighted",
    args	= {{ Object, "obj" },
		   { Bool, "yes_or_no" }}
}

generate_client {
    cname	= "object_moving_horizontally",
    lname	= "_internal_object_moving_horizontally",
    args	= {{ Object, "obj" }},
    ret		= { Bool, "ret", "!$" }
}

generate_server {
    cname	= "object_set_collision_is_player",
    args	= {{ Object, "obj" }}
}

generate_server {
    cname	= "object_set_collision_is_projectile",
    args	= {{ Object, "obj" }}
}

generate_server {
    cname	= "object_set_collision_is_ladder",
    args	= {{ Object, "obj" }}
}

generate_server {
    cname	= "object_set_collision_flags_string",
    lname	= "object_set_collision_flags",
    args	= {{ Object, "obj" },
		   { String, "flags" }}
}

generate_server {
    cname	= "object_set_collision_tag",
    args	= {{ Object, "obj" },
		   { ObjTag, "tag" }}
}

generate_server {
    cname	= "object_add_creation_field",
    args	= {{ Object, "obj" },
		   { String, "name" }}
}

generate_both {
    cname	= "object_set_update_hook",
    args	= {{ Object, "obj" },
		   { Int, "msecs" },
		   { Method, "method" }}
}

generate_both {
    cname	= "object_remove_update_hook",
    args	= {{ Object, "obj" }}
}

generate_client {
    cname 	= "object_add_layer",
    args  	= {{ Object, "obj" },
                   { StoreKey, "image" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret 	= { Int, "layerid" },
    success	= "lua_pushnumber(L, layerid); return 1;"
}

generate_client {
    cname	= "object_replace_layer",
    args	= {{ Object, "obj" },
		   { Int, "layerid" },
		   { StoreKey, "image" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_client {
    cname	= "object_move_layer",
    args	= {{ Object, "obj" },
		   { Int, "layerid" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_client {
    cname	= "object_hflip_layer",
    args	= {{ Object, "obj" },
		   { Int, "layerid" },
		   { Bool, "hflip" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_client {
    cname	= "object_rotate_layer",
    args	= {{ Object, "obj" },
		   { Int, "layerid" },
		   { Int, "angle" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_client {
    cname	= "object_remove_layer",
    args	= {{ Object, "obj" },
		   { Int, "layerid" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_client {
    cname	= "object_remove_all_layers",
    args	= {{ Object, "obj" }}
}

generate_client {
    cname 	= "object_add_light",
    args  	= {{ Object, "obj" },
                   { StoreKey, "image" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret 	= { Int, "lightid" },
    success	= "lua_pushnumber(L, lightid); return 1;"
}

generate_client {
    cname	= "object_replace_light",
    args	= {{ Object, "obj" },
		   { Int, "lightid" },
		   { StoreKey, "image" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_client {
    cname	= "object_move_light",
    args	= {{ Object, "obj" },
		   { Int, "lightid" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_client {
    cname	= "object_remove_light",
    args	= {{ Object, "obj" },
		   { Int, "lightid" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_client {
    cname	= "object_remove_all_lights",
    args	= {{ Object, "obj" }}
}

generate_server {
    cname	= "object_set_mask",
    args	= {{ Object, "obj" },
		   { Int, "masknumber" },
		   { StoreKey, "mask" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_server {
    cname	= "object_set_masks_centre",
    args	= {{ Object, "obj" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }}
}

generate_server {
    cname	= "object_remove_mask",
    args	= {{ Object, "obj" },
		   { Int, "masknumber" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate_server {
    cname	= "object_remove_all_masks",
    args	= {{ Object, "obj" }}
}


--------------
-- svgame.h --
--------------

generate_server {
    cname	= "svgame_spawn_object",
    lname	= "spawn_object",
    args	= {{ String, "typename" },
		   { Float, "x" },
		   { Float, "y" }},
    ret		= { Object, "obj", "!$" },
    success	= "lua_pushobject(L, obj); return 1;"
}

generate_server {
    cname	= "svgame_spawn_projectile",
    lname	= "spawn_projectile",
    check	= "sun[nN-]",
    args	= {{ String, "typename" },
		   { Object, "owner" },
		   { Float, "speed" },
		   { Float, "delta_angle", nil,
		     "$ = (lua_isnoneornil(L, $#) ? 0. : lua_tonumber(L, $#));" }},
    ret		= { Object, "obj", "!$" },
    success	= "lua_pushobject(L, obj); return 1;"
}

generate_server {
    cname	= "svgame_spawn_projectile_raw",
    lname	= "spawn_projectile_raw",
    args	= {{ String, "typename" },
		   { ObjId, "ownerid" },
		   { Float, "x" },
		   { Float, "y" },
		   { Float, "angle" },
		   { Float, "speed" }},
    ret		= { Object, "obj", "!$" },
    success	= "lua_pushobject(L, obj); return 1;"
}

generate_server {
    cname	= "svgame_spawn_blood",
    lname	= "spawn_blood_on_clients",
    args	= {{ Float, "x" },
		   { Float, "y" },
		   { Int, "nparticles" },
		   { Float, "spread" }}
}

generate_server {
    cname	= "svgame_spawn_sparks",
    lname	= "spawn_sparks_on_clients",
    args	= {{ Float, "x" },
		   { Float, "y" },
		   { Int, "nparticles" },
		   { Float, "spread" }}
}

generate_server {
    cname	= "svgame_spawn_respawn_particles",
    lname	= "spawn_respawn_particles_on_clients",
    args	= {{ Float, "x" },
		   { Float, "y" },
		   { Int, "nparticles" },
		   { Float, "spread" }}
}

generate_server {
    cname	= "svgame_spawn_blod",
    lname	= "spawn_blod_on_clients",
    args	= {{ Float, "x" },
		   { Float, "y" },
		   { Int, "nparticles" }}
}

generate_server {
    cname	= "svgame_spawn_explosion",
    lname	= "spawn_explosion_on_clients",
    args	= {{ String, "name" },
		   { Float, "x" },
		   { Float, "y" }}
}

generate_server {
    cname	= "svgame_spawn_blast",
    lname	= "spawn_blast",
    args	= {{ Float, "x" },
		   { Float, "y" },
	           { Float, "radius" },
	       	   { Int, "damage" },
	           { ClientId, "ownerid" }}
}

generate_server {
    cname	= "svgame_call_method_on_clients",
    lname	= "call_method_on_clients",
    check	= "us[sN-]",
    args	= {{ Object, "obj" },
		   { String, "method" },
		   { String, "arg", nil,
		     "$ = (lua_isnoneornil(L, $#) ? \"\" : lua_tostring(L, $#));" }}
}

generate_server {
    cname	= "svgame_object_would_collide_with_player_if_unhidden",
    lname	= "_internal_would_collide_with_player_if_unhidden",
    args	= {{ Object, "obj" }},
    ret		= { Int, "ret", "!$" }
}

generate_server {
    cname	= "svgame_tell_health",
    lname	= "_internal_tell_health",
    args	= {{ Object, "obj" },
		   { Int, "health" }}
}

generate_server {
    cname	= "svgame_tell_armour",
    lname	= "_internal_tell_armour",
    args	= {{ Object, "obj" },
		   { Int, "armour" }}
}

generate_server {
    cname	= "svgame_tell_ammo",
    lname	= "_internal_tell_ammo",
    args	= {{ Object, "obj" },
		   { Int, "ammo" }}
}

generate_server {
    cname	= "svgame_set_score",
    lname	= "set_score",
    args	= {{ ClientId, "clientid" },
		   { String, "score" }}
}

generate_server {
    cname	= "svgame_play_sound_on_clients",
    lname	= "play_sound_on_clients",
    args	= {{ Object, "obj" },
		   { String, "sample" }}
}

generate_server {
    cname	= "svgame_get_client_name",
    lname	= "get_client_name",
    args	= {{ ClientId, "clientid" }},
    ret		= { String, "ret", "!$" },
    success	= "lua_pushstring(L, ret); return 1;"
}

generate_server {
    cname	= "svgame_broadcast_text_message",
    lname	= "broadcast_text_message",
    args	= {{ String, "message" }}
}

generate_server {
    cname	= "svgame_send_text_message",
    lname	= "send_text_message",
    args	= {{ ClientId, "clientid" },
		   { String, "message" }}
}


--------------
-- client.h --
--------------

generate_client {
    cname	= "client_spawn_explosion",
    lname	= "spawn_explosion",
    args	= {{ String, "name" },
		   { Float, "x" },
		   { Float, "y" }}
}

generate_client {
    cname	= "client_play_sound",
    lname	= "play_sound",
    args	= {{ Object, "obj" },
		   { String, "sample" }}
}

generate_client {
    cname	= "client_set_camera",
    lname	= "_internal_set_camera",
    args	= {{ Bool, "pushable" },
		   { Int, "max_dist" }}
}


-- End of declarations

print (reg_init) print ()
print (unreg_init) print ()
print (reg_both) print ()
print (reg_server) print ()
print (reg_client)


-- bindgen.lua ends here
