-- silly.lua (generates silly.dat)

dofile ("tilehelp.lua")

images = {
    ["mascots"]	= "mascots.png",
    ["quake"] 	= "quake.png",
    ["other"] 	= "other.png"
}

make_tile_pack ("silly.dat", "silly/", images)

-- end of silly.lua
