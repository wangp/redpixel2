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
    for i,v in load_path do 
	if old_dofile (v.."/"..filename) then return 1 end
    end
    return nil
end


-- modules to load

local files = {
    "basic/basic-init.lua",
    "editor/editor-init.lua"
}

for _,filename in files do
    dofile (filename)
end
