-- bindgen.lua -- generate Lua <-> C bindings


-- Types that we should know about.
--	field 1: checkargs type char
--	field 2: C type
--	field 3: Lua to C conversion function
--	field 4: optional error check; $ is replaced by the variable name

ctypes = {
    -- builtin types
    Float	= { "n", "float", "tonumber" },
    Function 	= { "f", "lref_t", "ref" },
    Int	 	= { "n", "int", "tonumber" },
    String	= { "s", "const char *", "tostring" },
    -- custom and readability types
    Method   	= { "f", "lref_t", "ref" },
    Object	= { "t", "object_t *", "table_object", "!$" },
    StoreKey	= { "s", "const char *", "tostring" }
}

-- convenience
Float = "Float"
Function = "Function"
Int = "Int"
String = "String"
Method = "Method"
Object = "Object"
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
    local tab = "\t"
    local str

    function generate_check ()
	local str = ""
	for i,v in %args do
	    local typ = %ctypes[v[1]]
	    str = str..typ[1]
	end
	return str
    end

    -- function declaration
    str = "static int bind_"..(lname or cname).."(lua_State *L)\n{\n"

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
    str = str..tab..'if (!checkargs(L, "'..(check or generate_check())..'")) goto error;\n'

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
	    str = str..tab.."if ("..v[3]..") goto error;\n"
	elseif typ[4] then
	    str = str..tab.."if ("..gsub(typ[4], "%$", v[2])..") goto error;\n"
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
    str = str..tab..(success or "pushnumber(L, 1); return 1;").."\n"

    -- error clause
    str = str.."error:\n"..tab..(error or "pushnil(L); return 1;").."\n"

    str = str.."}\n"
    print (str)
end

reg = "#define DO_REGISTRATION(L) "

function generate (table)
    generate_code (table.lname, table.cname, table.check, table.args, 
		   table.ret, table.success, table.error)

    reg = reg..' \\\n\tlregister(L, "'..(table.lname or table.cname)..'", bind_'..(table.lname or table.cname)..');'
end


-- Begin output.

print ("/* Generated by bindgen.lua on "..date()..".  Do not edit.  */\n")

generate {
    lname	= "store_load",
    args	= {{ String, "filename" },
		   { String, "prefix" }},
    ret		= { Int, "ret", "$ < 0",
		    "$ = store_load_ex(filename, prefix, load_extended_datafile);" }
}

generate {
    lname	= "objtype_register",
    cname	= "objtypes_register",
    check	= "sss[fN-]",
    args	= {{ String, "type" },
    		   { String, "name" },
		   { String, "icon" },
		   { Method, "func", nil, 
		     "$ = ((isnil(L, $#) || isnull(L, $#)) ? LUA_NOREF : ref(L, $#));" }}
}

generate {
    cname	= "object_set_mass",
    args	= {{ Object, "obj" },
		   { Float, "mass" }}
}

generate {
    cname 	= "object_set_ramp",
    args 	= {{ Object, "obj" },
		   { Float, "rampamount" }}
}

generate {
    cname 	= "object_add_layer",
    args  	= {{ Object, "obj" },
                   { StoreKey, "image" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret 	= { Int, "layerid" },
    success	= "pushnumber(L, layerid); return 1;"
}

generate {
    cname	= "object_replace_layer",
    args	= {{ Object, "obj" },
		   { Int, "layerid" },
		   { StoreKey, "image" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate {
    cname	= "object_move_layer",
    args	= {{ Object, "obj" },
		   { Int, "layerid" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate {
    cname	= "object_remove_layer",
    args	= {{ Object, "obj" },
		   { Int, "layerid" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate {
    cname	= "object_remove_all_layers",
    args	= {{ Object, "obj" }}
}

generate {
    cname 	= "object_add_light",
    args  	= {{ Object, "obj" },
                   { StoreKey, "image" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret 	= { Int, "lightid" },
    success	= "pushnumber(L, lightid); return 1;"
}

generate {
    cname	= "object_replace_light",
    args	= {{ Object, "obj" },
		   { Int, "lightid" },
		   { StoreKey, "image" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate {
    cname	= "object_move_light",
    args	= {{ Object, "obj" },
		   { Int, "lightid" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate {
    cname	= "object_remove_light",
    args	= {{ Object, "obj" },
		   { Int, "lightid" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate {
    cname	= "object_remove_all_lights",
    args	= {{ Object, "obj" }}
}

generate {
    cname	= "object_set_mask",
    args	= {{ Object, "obj" },
		   { Int, "masknumber" },
		   { StoreKey, "mask" },
		   { Int, "xoffset" },
		   { Int, "yoffset" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate {
    cname	= "object_remove_mask",
    args	= {{ Object, "obj" },
		   { Int, "masknumber" }},
    ret		= { Int, "ret", "$ < 0" }
}

generate {
    cname	= "object_remove_all_masks",
    args	= {{ Object, "obj" }}
}

print (reg)


-- bindgen.lua ends here
