-- basic-weapon.lua


store_load ("basic/basic-weapon.dat", "/basic/weapon/")


local make_weapon_nonproxy_init
function make_weapon_nonproxy_init (weapon_to_give, respawn_secs)
    return
    function (self)
	self:set_collision_flags ("p")
	function self:collide_hook (player)
	    player:receive_weapon (weapon_to_give)
	    self:hide ()
	    self:set_update_hook (respawn_secs * 1000,
				  function (self)
				      self:show ()
				      self:remove_update_hook ()
				  end)
	end
    end
end


----------------------------------------------------------------------
--  Blaster
----------------------------------------------------------------------

Weapon {
    name = "basic-blaster",
    fire = function (player)
	spawn_projectile ("basic-blaster-projectile", player, 10)
	player.fire_delay = 5
    end
}

Objtype {
    category = "weapon",
    name = "basic-blaster",
    icon = "/basic/weapon/blaster/pickup",
    nonproxy_init = make_weapon_nonproxy_init ("basic-blaster", 10)
}

Objtype {
    category = "projectile",
    name = "basic-blaster-projectile",
    icon = "/basic/weapon/blaster/projectile",
    nonproxy_init = function (self)
	function self:collide_hook (obj)
	    obj:receive_damage (10)
	    self:destroy ()
	end
	function self:tile_collide_hook ()
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
    icon = "/basic/weapon/bow/pickup",
    nonproxy_init = make_weapon_nonproxy_init ("basic-bow", 10)
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
    icon = "/basic/weapon/ak/pickup",
    nonproxy_init = make_weapon_nonproxy_init ("basic-ak", 10)
}

Objtype {
    category = "weapon", 
    name = "basic-minigun", 
    icon = "/basic/weapon/minigun/pickup",
    nonproxy_init = make_weapon_nonproxy_init ("basic-minigun", 10)
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
    icon = "/basic/weapon/rpg/pickup",
    nonproxy_init = make_weapon_nonproxy_init ("basic-rpg", 10)
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

local shotgun_spread = PI / 96

Weapon {
    name = "basic-shotgun",
    fire = function (player)
	spawn_projectile ("basic-shotgun-projectile", player, 10, -2*shotgun_spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, -shotgun_spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, 0)
	spawn_projectile ("basic-shotgun-projectile", player, 10, shotgun_spread)
	spawn_projectile ("basic-shotgun-projectile", player, 10, 2*shotgun_spread)
	player.fire_delay = 50 * 0.4
    end
}

Objtype {
    category = "weapon", 
    name = "basic-shotgun", 
    icon = "/basic/weapon/shotgun/pickup",
    nonproxy_init = make_weapon_nonproxy_init ("basic-shotgun", 10)
}

Objtype {
    category = "weapon", 
    name = "basic-shell", 
    icon = "/basic/weapon/ammo/shell"
}

Objtype {
    name = "basic-shotgun-projectile",
    icon = "/basic/weapon/shotgun/projectile",
    nonproxy_init = function (self)
	function self:collide_hook (obj)
	    obj:receive_damage (10)
	    self:destroy ()
	end
	function self:tile_collide_hook ()
	    self:destroy ()
	end	
    end
}


----------------------------------------------------------------------
--  Sniper rifle
----------------------------------------------------------------------

Weapon {
    name = "basic-rifle",
    fire = function (player)
	spawn_projectile ("basic-rifle-projectile", player, 15)
	player.fire_delay = 50 * 0.5
    end
}

Objtype {
    category = "weapon", 
    name = "basic-rifle", 
    icon = "/basic/weapon/rifle/pickup",
    nonproxy_init = make_weapon_nonproxy_init ("basic-rifle", 10)
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

Objtype {
    category = "weapon", 
    name = "basic-mine", 
    icon = "/basic/weapon/mine/pickup"
}
