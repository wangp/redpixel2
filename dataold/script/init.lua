-- init.lua


-- Paths to search for files.
-- XXX: make this use path.c stuff

load_path = {
    "data",
    "/usr/local/share/redpixel2",
    "/usr/share/redpixel2"
}


-- Enhanced `store_load' function.

function store_load (filename, prefix)
    for i,v in load_path do 
	if %store_load (v.."/"..filename, prefix) then return 1 end
    end
    return nil
end


-- Files to load.

local files = {
    "base.lua",
    "bow.lua",
    "enhance.lua",
    "explode.lua",
    "food.lua",
    "minigun.lua",
    "offc-obj.lua",
    "player.lua",
    "rpg.lua",
    "shotgun.lua"
}

for _,filename in files do
    for _,path in load_path do
	if dofile (path.."/script/"..filename) then break end
    end
end
