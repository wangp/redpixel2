-- basic-weapon.lua


store_load ("basic/basic-weapon.dat", "/basic/weapon/")



----------------------------------------------------------------------
--  Generalised declarators
----------------------------------------------------------------------


-- Declare a new weapon type with a standard firing routine, and standard ammo
-- counts.
local Weapon_With_Firer = function (t)
    return Weapon (t, {
	can_fire = function (player)
	    return player:ammo (t.ammo_type) > 0
	end,
	fire = function (player)
	    spawn_projectile (t.projectile, player, t.projectile_speed)
	    player.fire_delay = t.fire_delay_secs * 50
	    player:deduct_ammo (t.ammo_type)
	end
    })
end


-- Declare a new weapon or ammo pickup, which respawns a while after being
-- picked up.
local Standard_Pickup = function (t)
    if t.alias then add_alias (t.name, t.alias) end
    return Respawning_Item (t, {
	category = "weapon",
	collide_hook = function (self, player)
	    if t.weapon_to_give then
		player:receive_weapon (t.weapon_to_give)
	    end
	    if t.ammo_to_give then
		player:receive_ammo (t.ammo_to_give, t.ammo_amount)
	    end
	end
    })
end


-- Declare a new projectile type.  This only handles standard-style
-- projectiles, like bullets.
local Standard_Projectile = function (t)
    if t.alias then add_alias (t.name, t.alias) end
    return Objtype (t, {
	nonproxy_init = function (self)
	    function self:collide_hook (obj)
		obj:receive_damage (t.damage)
		self:destroy ()
	    end
	    function self:tile_collide_hook ()
		self:destroy ()
	    end	
	end
    })
end


----------------------------------------------------------------------
--  Blaster
----------------------------------------------------------------------

Weapon {
    name = "basic-blaster", 
    arm_anim = {
	"/basic/weapon/blaster/1arm000",
	"/basic/weapon/blaster/1arm001",
	"/basic/weapon/blaster/1arm002",
	"/basic/weapon/blaster/1arm003",
	"/basic/weapon/blaster/1arm004";
	cx = 0, cy = 3, tics = 2
    },
    fire = function (player)
	spawn_projectile ("basic-blaster-projectile", player, 10)
	player.fire_delay = 50 * 0.1
    end
}

Standard_Pickup {
    name = "basic-blaster",
    icon = "/basic/weapon/blaster/pickup",
    weapon_to_give = "basic-blaster",
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-blaster-projectile",
    alias = "~bp",
    icon = "/basic/weapon/blaster/projectile",
    damage = 10,
    proxy_init = function (self)
	self:rotate_layer (0, radian_to_bangle (self.angle))
    end
}


----------------------------------------------------------------------
--  Bow and arrow
----------------------------------------------------------------------

Standard_Pickup {
    name = "basic-bow",
    icon = "/basic/weapon/bow/pickup",
    weapon_to_give = "basic-bow",
    ammo_to_give = "basic-arrow",
    ammo_amount = 10,
    respawn_secs = 10
}

Standard_Pickup {
    name = "basic-arrow",
    icon = "/basic/weapon/ammo/arrow", 
    ammo_to_give = "basic-arrow",
    ammo_amount = 10,
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-arrow-projectile",
    alias = "~ap",
    icon = "/basic/weapon/bow/projectile",
    damage = 10
}


----------------------------------------------------------------------
--  Bullet weapons
----------------------------------------------------------------------

Standard_Pickup {
    name = "basic-bullet",
    icon = "/basic/weapon/ammo/bullet",
    ammo_to_give = "basic-bullet",
    ammo_amount = 30,
    respawn_secs = 10
}

Weapon_With_Firer {
    name = "basic-ak",
    ammo_type = "basic-bullet",
    projectile = "basic-ak-projectile",
    projectile_speed = 12,
    fire_delay_secs = 0.12,
    arm_anim = {
	"/basic/weapon/ak/2arm000",
	"/basic/weapon/ak/2arm001",
	"/basic/weapon/ak/2arm002",
	"/basic/weapon/ak/2arm003",
	"/basic/weapon/ak/2arm004",
	"/basic/weapon/ak/2arm005",
	"/basic/weapon/ak/2arm006";
	cx = 0, cy = 3
    }
}

Standard_Pickup {
    name = "basic-ak",
    icon = "/basic/weapon/ak/pickup",
    weapon_to_give = "basic-ak",
    ammo_to_give = "basic-bullet",
    ammo_amount = 10,
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-ak-projectile",
    alias = "~Ap",
    icon = "/basic/weapon/shotgun/projectile", -- XXX
    damage = 10
}

Weapon {
    name = "basic-minigun",
    ammo_type = "basic-bullet",
    can_fire = function (player)
	return player:ammo ("basic-bullet") > 0
    end,
    fire = function (player)
	spawn_projectile ("basic-minigun-projectile", player, 12,
			  ((random(10) - 5) / 10) * (PI/48))
	player.fire_delay = 0.05 * 50
	player:deduct_ammo ("basic-bullet")
    end,
    arm_anim = {
	"/basic/weapon/minigun/2arm000",
	"/basic/weapon/minigun/2arm001",
	"/basic/weapon/minigun/2arm002",
	"/basic/weapon/minigun/2arm003",
	"/basic/weapon/minigun/2arm004",
	"/basic/weapon/minigun/2arm005",
	"/basic/weapon/minigun/2arm006";
	cx = 0, cy = 1, tics = 2
    }
}

Standard_Pickup {
    name = "basic-minigun",
    icon = "/basic/weapon/minigun/pickup",
    weapon_to_give = "basic-minigun",
    ammo_to_give = "basic-bullet",
    ammo_amount = 10,
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-minigun-projectile",
    alias = "~mp",
    icon = "/basic/weapon/shotgun/projectile", -- XXX
    damage = 10
}


----------------------------------------------------------------------
--  Rocket weapons
----------------------------------------------------------------------

Standard_Pickup {
    name = "basic-rpg",
    icon = "/basic/weapon/rpg/pickup",
    weapon_to_give = "basic-rpg",
    ammo_to_give = "basic-rocket",
    ammo_amount = 1,
    respawn_secs = 10
}

Standard_Pickup {
    name = "basic-rocket",
    icon = "/basic/weapon/ammo/rocket",
    ammo_to_give = "basic-rocket",
    ammo_amount = 10,
    respawn_secs = 10,
}

Standard_Projectile {
    name = "basic-rocket-projectile",
    alias = "~rp",
    icon = "/basic/weapon/rpg/projectile",
    damage = 10
}


----------------------------------------------------------------------
--  Shotgun
----------------------------------------------------------------------

Weapon {
    name = "basic-shotgun",
    can_fire = function (player)
	return player:ammo ("basic-shell") > 0
    end,
    fire = function (player)
	local spread = PI / 96
	spawn_projectile ("basic-shotgun-projectile", player, 10, -2 * spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, -spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, 0)
	spawn_projectile ("basic-shotgun-projectile", player, 10, spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, 2 * spread)
	player.fire_delay = 50 * 0.4
	player:deduct_ammo ("basic-shell")
    end,
    arm_anim = {
	"/basic/weapon/shotgun/2arm000",
	"/basic/weapon/shotgun/2arm001",
	"/basic/weapon/shotgun/2arm002",
	"/basic/weapon/shotgun/2arm003";
	cx = 1, cy = 4, tics = 4
    }
}

Standard_Pickup {
    name = "basic-shotgun",
    icon = "/basic/weapon/shotgun/pickup",
    weapon_to_give = "basic-shotgun",
    ammo_to_give = "basic-shell",
    ammo_amount = 10,
    respawn_secs = 10
}

Standard_Pickup {
    name = "basic-shell",
    icon = "/basic/weapon/ammo/shell",
    ammo_to_give = "basic-shell",
    ammo_amount = 10,
    respawn_secs = 10,
}

Standard_Projectile {
    name = "basic-shotgun-projectile",
    alias = "~sp",
    icon = "/basic/weapon/shotgun/projectile",
    damage = 10
}


----------------------------------------------------------------------
--  Sniper rifle
----------------------------------------------------------------------

Weapon_With_Firer {
    name = "basic-rifle",
    ammo_type = "basic-slug",
    projectile = "basic-rifle-projectile",
    projectile_speed = 15,
    fire_delay_secs = 0.5,
    arm_anim = {
	"/basic/weapon/rifle/1arm000",
	"/basic/weapon/rifle/1arm001",
	"/basic/weapon/rifle/1arm002";
	cx = -2, cy = 4
    },
    client_switch_to_hook = function ()
	_internal_set_camera (1, 300)
    end
}

Standard_Pickup {
    name = "basic-rifle",
    icon = "/basic/weapon/rifle/pickup",
    weapon_to_give = "basic-rifle",
    ammo_to_give = "basic-slug",
    ammo_amount = 2,
    respawn_secs = 10
}

Objtype {
    name = "basic-rifle-projectile",
    icon = "/basic/weapon/shotgun/projectile", -- XXX
    nonproxy_init = function (self)
        self:set_collision_flags ("pn")
	function self:collide_hook (obj)
	    obj:receive_damage (50)
	    self:destroy ()
	end
    end
}

add_alias ("basic-rifle-projectile", "~Rp")


----------------------------------------------------------------------
--  Mines
----------------------------------------------------------------------

Standard_Pickup {
    name = "basic-mine",
    icon = "/basic/weapon/mine/pickup",
    respawn_secs = 10
}


----------------------------------------------------------------------
--  Weapon switch order
----------------------------------------------------------------------

weapon_order = {
    "basic-blaster",
    "basic-shotgun",
    "basic-ak",
    "basic-minigun",
    "basic-bow",
    "basic-rpg",
    "basic-rifle"
}
