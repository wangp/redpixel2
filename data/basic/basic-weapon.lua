-- basic-weapon.lua


store_load ("basic/basic-weapon.dat", "/basic/weapon/")



----------------------------------------------------------------------
--  Generalised declarators
----------------------------------------------------------------------


-- Declare a new weapon type with a standard firing routine.
local Weapon_With_Firer = function (t)
    return Weapon (merge (t, { fire = function (player)
			           spawn_projectile (t.projectile, player, t.projectile_speed)
				   player.fire_delay = t.fire_delay_secs * 50
			       end }))
end


-- Declare a new weapon pickup, which respawns a while after being
-- picked up.
local Weapon_Pickup = function (t)
    return Objtype (merge (t, { category = "weapon",
			        nonproxy_init = function (self)
				    self:set_collision_flags ("p")
				    function self:collide_hook (player)
					player:receive_weapon (t.weapon_to_give)
					self:hide_and_respawn_later (t.respawn_secs * 1000)
				    end
				end }))
end


-- Declare some weapon ammunition, which respawns a while after being
-- picked up.
local Weapon_Ammo = function (t)
    return Objtype (merge (t, { category = "weapon",
			        nonproxy_init = function (self)
				    self:set_collision_flags ("p")
				    function self:collide_hook (obj)
					self:hide_and_respawn_later (t.respawn_secs * 1000)
				    end
				end }))
end


-- Declare a new projectile type.  This only handles standard-style
-- projectiles, like bullets.
local Standard_Projectile = function (t)
    return Objtype (merge (t, { nonproxy_init = function (self)
				    function self:collide_hook (obj)
					obj:receive_damage (t.damage)
					self:destroy ()
				    end
				    function self:tile_collide_hook ()
					self:destroy ()
				    end	
				end }))
end


----------------------------------------------------------------------
--  Blaster
----------------------------------------------------------------------

Weapon_With_Firer {
    name = "basic-blaster", 
    projectile = "basic-blaster-projectile",
    projectile_speed = 10,
    fire_delay_secs = 0.1,
    arm_anim = {
	"/basic/weapon/blaster/1arm000",
	"/basic/weapon/blaster/1arm001",
	"/basic/weapon/blaster/1arm002",
	"/basic/weapon/blaster/1arm003";
	cx = 0, 
	cy = 3
    }
}

Weapon_Pickup {
    name = "basic-blaster",
    icon = "/basic/weapon/blaster/pickup",
    weapon_to_give = "basic-blaster",
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-blaster-projectile",
    icon = "/basic/weapon/blaster/projectile",
    damage = 10,
    proxy_init = function (self)
	self:rotate_layer (0, radian_to_bangle (self.angle))
    end
}


----------------------------------------------------------------------
--  Bow and arrow
----------------------------------------------------------------------

Weapon_Pickup {
    name = "basic-bow",
    icon = "/basic/weapon/bow/pickup",
    weapon_to_give = "basic-bow",
    respawn_secs = 10
}

Weapon_Ammo {
    name = "basic-arrow",
    icon = "/basic/weapon/ammo/arrow", 
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-arrow-projectile",
    icon = "/basic/weapon/bow/projectile",
    damage = 10
}


----------------------------------------------------------------------
--  Bullet weapons
----------------------------------------------------------------------

Weapon_Ammo {
    name = "basic-bullet",
    icon = "/basic/weapon/ammo/bullet",
    respawn_secs = 10
}

Weapon_With_Firer {
    name = "basic-ak",
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

Weapon_Pickup {
    name = "basic-ak",
    icon = "/basic/weapon/ak/pickup",
    weapon_to_give = "basic-ak",
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-ak-projectile",
    icon = "/basic/weapon/shotgun/projectile", -- XXX
    damage = 10
}

Weapon {
    name = "basic-minigun",
    fire = function (player)
	spawn_projectile ("basic-minigun-projectile", player, 12,
			  ((random(10) - 5) / 10) * (PI/48))
	player.fire_delay = 0.05 * 50
    end,
    arm_anim = {
	"/basic/weapon/minigun/2arm000",
	"/basic/weapon/minigun/2arm001",
	"/basic/weapon/minigun/2arm002",
	"/basic/weapon/minigun/2arm003",
	"/basic/weapon/minigun/2arm004",
	"/basic/weapon/minigun/2arm005",
	"/basic/weapon/minigun/2arm006";
	cx = 0, cy = 1
    }
}

Weapon_Pickup {
    name = "basic-minigun",
    icon = "/basic/weapon/minigun/pickup",
    weapon_to_give = "basic-minigun",
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-minigun-projectile",
    icon = "/basic/weapon/shotgun/projectile", -- XXX
    damage = 10
}


----------------------------------------------------------------------
--  Rocket weapons
----------------------------------------------------------------------

Weapon_Pickup {
    name = "basic-rpg",
    icon = "/basic/weapon/rpg/pickup",
    weapon_to_give = "basic-rpg",
    respawn_secs = 10
}

Weapon_Ammo {
    name = "basic-rocket",
    icon = "/basic/weapon/ammo/rocket",
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-rocket-projectile",
    icon = "/basic/weapon/rpg/projectile",
    damage = 10
}


----------------------------------------------------------------------
--  Shotgun
----------------------------------------------------------------------

Weapon {
    name = "basic-shotgun",
    fire = function (player)
	local spread = PI / 96
	spawn_projectile ("basic-shotgun-projectile", player, 10, -2 * spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, -spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, 0)
	spawn_projectile ("basic-shotgun-projectile", player, 10, spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, 2 * spread)
	player.fire_delay = 50 * 0.4
    end,
    arm_anim = {
	"/basic/weapon/shotgun/2arm000",
	"/basic/weapon/shotgun/2arm001",
	"/basic/weapon/shotgun/2arm002",
	"/basic/weapon/shotgun/2arm003";
	cx = 1, cy = 4
    }
}

Weapon_Pickup {
    name = "basic-shotgun",
    icon = "/basic/weapon/shotgun/pickup",
    weapon_to_give = "basic-shotgun",
    respawn_secs = 10
}

Weapon_Ammo {
    name = "basic-shell",
    icon = "/basic/weapon/ammo/shell",
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-shotgun-projectile",
    icon = "/basic/weapon/shotgun/projectile",
    damage = 10
}


----------------------------------------------------------------------
--  Sniper rifle
----------------------------------------------------------------------

Weapon_With_Firer {
    name = "basic-rifle",
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

Weapon_Pickup {
    name = "basic-rifle",
    icon = "/basic/weapon/rifle/pickup",
    weapon_to_give = "basic-rifle",
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


----------------------------------------------------------------------
--  Mines
----------------------------------------------------------------------

Weapon_Ammo {
    name = "basic-mine",
    icon = "/basic/weapon/mine/pickup",
    respawn_secs = 10
}
