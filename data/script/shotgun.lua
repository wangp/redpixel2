-- shotgun.lua

local item_shotgun = {
    player_touched = function (self, player)
	object_destroy (self)
    end
}

local item_shell = {
}

function __module__init ()
    local prefix = "/weapon/shotgun/"

    store_load ("object/shotgun.dat", prefix)
    object_type_register ("shotgun",       %item_shotgun, "item", prefix .. "pickup")
    object_type_register ("shotgun-shell", %item_shell,   "item", prefix .. "ammo")
end
