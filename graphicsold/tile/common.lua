-- common.lua (generates common.dat)

dofile ("tilehelp.lua")

images = {
    ["ladder"]	= "ladder.png"
}

make_tile_pack ("common.dat", "common/", images)

-- end of common.lua
