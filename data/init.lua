-- init.lua


-- path stuff

local load_path = {
    "data",
    "/usr/local/share/redpixel2",
    "/usr/share/redpixel2"
}

local old_store_load = store_load
function store_load (filename, prefix)
    for i,v in load_path do 
	if old_store_load (v.."/"..filename, prefix) then return 1 end
    end
    return nil
end

local old_tiles_load = tiles_load
function tiles_load (filename, prefix)
    for i,v in load_path do 
	if old_tiles_load (v.."/"..filename, prefix) then return 1 end
    end
    return nil
end

local old_lights_load = lights_load
function lights_load (filename, prefix)
    for i,v in load_path do 
	if old_lights_load (v.."/"..filename, prefix) then return 1 end
    end
    return nil
end

local old_dofile = dofile
function dofile (filename)
    -- local dbg = print
    local dbg = function (s) return end
    dbg ("Doing file "..filename)
    for i,v in load_path do 
	dbg (" trying "..v.."/"..filename)
	local chunk = loadfile (v.."/"..filename)
	if chunk then
	    chunk ()
	    return 1
	end
    end
    return nil
end


-- object type name aliases (long name -> short name)

object_alias = {}
reverse_object_alias = {}

function add_alias (long, short)
    if reverse_object_alias[short] then
	print ("Alias short-name "..short.." already exists, ignoring")
    else
	object_alias[long] = short
	reverse_object_alias[short] = long
    end
end


-- modules to load

local files = {
    "basic/basic-init.lua",
    "frontend/frontend-menu.lua"
}

for _,filename in files do
    dofile (filename)
end
