-- basic-weapon.lua


store_load ("basic/basic-weapon.dat", "/basic/weapon/")



----------------------------------------------------------------------
--  Generalised declarators
----------------------------------------------------------------------


-- Declare a new weapon type with a standard firing routine.
local Weapon_With_Firer = function (name, projectile, speed, fire_delay_secs)
    return Weapon {
	name = name,
	fire = function (player)
	    spawn_projectile (projectile, player, speed)
	    player.fire_delay = fire_delay_secs * 50
	end
    }
end


-- Declare a new weapon pickup, which respawns a while after being
-- picked up.
local Weapon_Pickup = function (name, icon, weapon_to_give, respawn_secs)
    return Objtype {
	category = "weapon",
	name = name,
	icon = icon,
	nonproxy_init = function (self)
	    self:set_collision_flags ("p")
	    function self:collide_hook (player)
		player:receive_weapon (weapon_to_give)
		self:hide_and_respawn_later (respawn_secs * 1000)
	    end
	end
    }
end


-- Declare some weapon ammunition, which respawns a while after being
-- picked up.
local Weapon_Ammo = function (name, icon, respawn_secs)
    return Objtype {
	category = "weapon",
	name = name,
	icon = icon,
	nonproxy_init = function (self)
	    self:set_collision_flags ("p")
	    function self:collide_hook (obj)
		self:hide_and_respawn_later (respawn_secs * 1000)
	    end
	end
    }
end


-- Declare a new projectile type.  This only handles standard-style
-- projectiles, like bullets.
local Standard_Projectile = function (name, icon, damage, proxy_init)
    return Objtype {
	name = name,
	icon = icon,
	nonproxy_init = function (self)
	    function self:collide_hook (obj)
		obj:receive_damage (damage)
		self:destroy ()
	    end
	    function self:tile_collide_hook ()
		self:destroy ()
	    end	
	end,
	proxy_init = proxy_init
    }
end


----------------------------------------------------------------------
--  Blaster
----------------------------------------------------------------------

Weapon_With_Firer ("basic-blaster", "basic-blaster-projectile", 10, 0.1)

Weapon_Pickup ("basic-blaster", "/basic/weapon/blaster/pickup",
	       "basic-blaster", 10)

Standard_Projectile ("basic-blaster-projectile",
		     "/basic/weapon/blaster/projectile",
		     10,
		     -- proxy_init
		     function (self)
			 self:rotate_layer (0, radian_to_bangle (self.angle))
		     end)


----------------------------------------------------------------------
--  Bow and arrow
----------------------------------------------------------------------

Weapon_Pickup ("basic-bow", "/basic/weapon/bow/pickup",
	       "basic-bow", 10)

Weapon_Ammo ("basic-arrow", "/basic/weapon/ammo/arrow", 10)

Standard_Projectile ("basic-arrow-projectile",
		     "/basic/weapon/bow/projectile",
		     10)


----------------------------------------------------------------------
--  Bullet weapons
----------------------------------------------------------------------

Weapon_Ammo ("basic-bullet", "/basic/weapon/ammo/bullet", 10)

Weapon_With_Firer ("basic-ak", "basic-ak-projectile", 12, 0.12)

Weapon_Pickup ("basic-ak", "/basic/weapon/ak/pickup",
	       "basic-ak", 10)

Standard_Projectile ("basic-ak-projectile",
		     "/basic/weapon/shotgun/projectile", -- XXX
		     10)

Weapon {
    name = "basic-minigun",
    fire = function (player)
	spawn_projectile ("basic-minigun-projectile", player, 12,
			  ((random(10) - 5) / 10) * (PI/48))
	player.fire_delay = 0.05 * 50
    end
}

Weapon_Pickup ("basic-minigun", "/basic/weapon/minigun/pickup",
	       "basic-minigun", 10)

Standard_Projectile ("basic-minigun-projectile",
		     "/basic/weapon/shotgun/projectile", -- XXX
		     10)


----------------------------------------------------------------------
--  Rocket weapons
----------------------------------------------------------------------

Weapon_Pickup ("basic-rpg", "/basic/weapon/rpg/pickup",
	       "basic-rpg", 10)

Weapon_Ammo ("basic-rocket", "/basic/weapon/ammo/rocket", 10)

Standard_Projectile ("basic-rocket-projectile",
		     "/basic/weapon/rpg/projectile",
		     10)


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
    end
}

Weapon_Pickup ("basic-shotgun", "/basic/weapon/shotgun/pickup",
	       "basic-shotgun", 10)

Weapon_Ammo ("basic-shell", "/basic/weapon/ammo/shell", 10)

Standard_Projectile ("basic-shotgun-projectile",
		     "/basic/weapon/shotgun/projectile",
		     10)


----------------------------------------------------------------------
--  Sniper rifle
----------------------------------------------------------------------

Weapon_With_Firer ("basic-rifle", "basic-rifle-projectile", 15, 0.5)

Weapon_Pickup ("basic-rifle", "/basic/weapon/rifle/pickup",
	       "basic-rifle", 10)

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

Weapon_Ammo ("basic-mine", "/basic/weapon/mine/pickup", 10)
