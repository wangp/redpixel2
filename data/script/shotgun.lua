-- shotgun.lua

local init_shotgun = function (self)
    self.player_touched = function (self)
-- 	object_destroy (self)
    end
end

-- Module init.

store_load ("object/shotgun.dat", "/weapon/shotgun/")
objtype_register ("item", "shotgun", "/weapon/shotgun/pickup", init_shotgun)
objtype_register ("item", "shotgun-shell", "/weapon/shotgun/ammo")
