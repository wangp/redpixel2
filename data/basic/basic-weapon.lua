-- basic-weapon.lua


store_load ("basic/basic-weapon.dat", "/basic/weapon/")


----------------------------------------------------------------------
--  Blaster
----------------------------------------------------------------------

Weapon {
    name = "basic-blaster",
    can_fire = function (player)
	return 1
    end,
    fire = function (player)
	spawn_projectile ("basic-blaster-projectile", player, 10)
	player.fire_delay = 5
    end
}

Objtype {
    category = "weapon",
    name = "basic-blaster",
    icon = "/basic/weapon/blaster/pickup",
    nonproxy_init = function (self)
	self:set_collision_flags ("p")
        function self.collide_hook (self, player)
	    player:receive_weapon ("basic-blaster")
	    self:destroy ()
	end
    end
}

Objtype {
    category = "projectile",
    name = "basic-blaster-projectile",
    icon = "/basic/weapon/blaster/projectile",
    nonproxy_init = function (self)
	function self.collide_hook (self, obj)
	    obj:receive_damage (10)
	    self:destroy ()
	end
	function self.tile_collide_hook (self)
	    self:destroy ()
	end
    end,
    proxy_init = function (self)
	self:rotate_layer (0, radian_to_bangle (self.angle))
    end
}


----------------------------------------------------------------------
--  Bow and arrow
----------------------------------------------------------------------

Objtype {
    category = "weapon",
    name = "basic-bow",
    icon = "/basic/weapon/bow/pickup"
}

Objtype {
    category = "weapon",
    name = "basic-arrow",
    icon = "/basic/weapon/ammo/arrow"
}

Objtype {
    category = "projectile",
    name = "basic-arrow-projectile",
    icon = "/basic/weapon/bow/projectile"
}


----------------------------------------------------------------------
--  Bullet weapons
----------------------------------------------------------------------

Objtype {
    category = "weapon",
    name = "basic-ak",
    icon = "/basic/weapon/ak/pickup"
}

Objtype {
    category = "weapon", 
    name = "basic-minigun", 
    icon = "/basic/weapon/minigun/pickup"
}

Objtype {
    category = "weapon", 
    name = "basic-bullet", 
    icon = "/basic/weapon/ammo/bullet"
}


----------------------------------------------------------------------
--  Rocket weapons
----------------------------------------------------------------------

Objtype {
    category = "weapon", 
    name = "basic-rpg", 
    icon = "/basic/weapon/rpg/pickup"
}

Objtype {
    category = "weapon", 
    name = "basic-rocket", 
    icon = "/basic/weapon/ammo/rocket"
}

Objtype {
    category = "projectile",
    name = "basic-rocket-projectile", 
    icon = "/basic/weapon/rpg/projectile"
}


----------------------------------------------------------------------
--  Shotgun
----------------------------------------------------------------------

Objtype {
    category = "weapon", 
    name = "basic-shotgun", 
    icon = "/basic/weapon/shotgun/pickup"
}

Objtype {
    category = "weapon", 
    name = "basic-shell", 
    icon = "/basic/weapon/ammo/shell"
}


----------------------------------------------------------------------
--  Sniper rifle
----------------------------------------------------------------------

Objtype {
    category = "weapon", 
    name = "basic-rifle", 
    icon = "/basic/weapon/rifle/pickup"
}


----------------------------------------------------------------------
--  Mines
----------------------------------------------------------------------

Objtype {
    category = "weapon", 
    name = "basic-mine", 
    icon = "/basic/weapon/mine/pickup"
}
