-- shotgun.lua

local weapon_shotgun = {
    fire = function (self, player, x, y, angle)
	-- spawn projectiles from x, y, at angle
    end
}

local pickup_shotgun = {
    spawn = function ()
    end,

    player_touch = function (self, player)
	-- give player shells
	-- give player shotgun
    end
}

local pickup_shell = {
    player_touch = function (self, player)
	-- give player shells
    end
}


function __module__init ()
    local prefix = "/weapons/shotgun/"

    if store_load ("objects/shotgun.dat", prefix) == 0 then
    --  register_weapon (weapon_shotgun)
	register_object (%pickup_shotgun, "item", prefix .. "pickup")
        register_object (%pickup_shell,   "item", prefix .. "ammo")
    end
end
