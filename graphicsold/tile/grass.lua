-- grass.lua (generates grass.dat)

dofile ("tilehelp.lua")

images = {
    ["dirt"]		= "dirt.png",
    ["dirt/fg"]		= "dirt-fg.png",
    ["grass"]       	= "grass.png",
    ["grass/fg"]	= "grass-fg.png"
}

make_tile_pack ("grass.dat", "grass/", images)

-- end of grass.lua
