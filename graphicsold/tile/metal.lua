-- metal.lua (generates metal.dat)

dofile ("tilehelp.lua")

images = {
    ["marble"]  = "marble.png",
    ["steel"]	= "steel-en.png"
}

make_tile_pack ("metal.dat", "metal/", images)

-- end of metal.lua
