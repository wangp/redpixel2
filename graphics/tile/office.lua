-- office.lua (generates office.dat)

dofile ("tilehelp.lua")

images = {
    ["book"]	 = "book.png",
    ["desk"]	 = "desk.png",
    ["chair"]	 = "chair.png",
    ["computer"] = "computer.png"
}

make_tile_pack ("office.dat", "office/", images)

-- end of office.lua
