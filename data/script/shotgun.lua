-- shotgun.lua

local item_shotgun = {
}

local item_shell = {
}

function __module__init ()
    local prefix = "/weapon/shotgun/"

    store_load ("object/shotgun.dat", prefix)
    register_object ("shotgun",       %item_shotgun, "item", prefix .. "pickup")
    register_object ("shotgun-shell", %item_shell,   "item", prefix .. "ammo")
end
