-- brick.lua (generates brick.dat)

dofile ("tilehelp.lua")

images = {
    ["bricks"]		= "bricks.png",
    ["colour"]		= "change-colour.png",
    ["brightness"]	= "change-brightness.png",
    ["dark"]		= "bricks-dark.png"
}

make_tile_pack ("brick.dat", "brick/", images)

-- end of brick.lua
