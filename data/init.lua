-- init.lua


-- path stuff

local load_path = {
    "data",
    "/usr/local/share/redpixel2",
    "/usr/share/redpixel2"
}

function store_load (filename, prefix)
    for i,v in %load_path do 
	if %store_load (v.."/"..filename, prefix) then return 1 end
    end
    return nil
end

function tiles_load (filename, prefix)
    for i,v in %load_path do 
	if %tiles_load (v.."/"..filename, prefix) then return 1 end
    end
    return nil
end

function lights_load (filename, prefix)
    for i,v in %load_path do 
	if %lights_load (v.."/"..filename, prefix) then return 1 end
    end
    return nil
end

function dofile (filename)
    for i,v in %load_path do 
	if %dofile (v.."/"..filename) then return 1 end
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
