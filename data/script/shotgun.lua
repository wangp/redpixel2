-- shotgun.lua

local item_shotgun = {
}

local item_shell = {
}

function __module__init ()
    local prefix = "/weapon/shotgun/"

    if store_load ("object/shotgun.dat", prefix) == 0 then
	register_object ("shotgun", 	  %item_shotgun, "item", prefix .. "pickup")
        register_object ("shotgun-shell", %item_shell,   "item", prefix .. "ammo")
    end
end
