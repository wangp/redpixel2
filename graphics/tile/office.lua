-- office.lua (generates office.dat)

dofile ("tilehelp.lua")

images = {
    ["books"]	 = "books.png",
    ["desk"]	 = "desk.png",
    ["computer"] = "computer.png"
}

make_tile_pack ("office.dat", "office/", images)

-- end of office.lua
