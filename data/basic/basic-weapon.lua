-- basic-weapon.lua


store_load ("basic/basic-weapon.dat", "/basic/weapon/")



----------------------------------------------------------------------
--  Generalised declarators
----------------------------------------------------------------------


-- Declare a new weapon type with a standard firing routine, and standard ammo
-- counts.
local Weapon_With_Firer = function (t)
    return Weapon (t, {
	fire = function (player)
	    local proj = spawn_projectile (t.projectile, player, t.projectile_speed)
	    player:set_fire_delay (t.fire_delay_secs)
	    player:deduct_ammo (t.ammo_type)
	    return proj
	end
    })
end


-- Declare a new weapon or ammo pickup, which respawns a while after being
-- picked up.
local Standard_Pickup = function (t)
    return Respawning_Item (t, {
	category = "weapon",
	collide_hook = function (self, player)
	    if t.weapon_to_give then
		player:receive_weapon (t.weapon_to_give)
	    end
	    if t.ammo_to_give then
		player:receive_ammo (t.ammo_to_give, t.ammo_amount)
	    end
	    play_sound_on_clients (self, t.sound or "/basic/weapon/generic-pickup-sound")
	end
    })
end


-- Declare a new projectile type.  This only handles standard-style
-- projectiles, like bullets.
local Standard_Projectile = function (t)
    return Objtype (t, {
	nonproxy_init = function (self)
	    object_set_collision_is_projectile (self)
	    if t.dot_mask then
		self:set_mask (mask_main,   "/basic/weapon/single-dot-mask", 0, 0)
		self:set_mask (mask_top,    "/basic/weapon/single-dot-mask", 0, 0)
		self:set_mask (mask_bottom, "/basic/weapon/single-dot-mask", 0, 0)
		self:set_mask (mask_left,   "/basic/weapon/single-dot-mask", 0, 0)
		self:set_mask (mask_right,  "/basic/weapon/single-dot-mask", 0, 0)
	    end
	    self.damage = t.damage
	    function self:collide_hook (obj)
		obj:receive_damage (self.damage, self.owner, self.x, self.y)
		self:set_stale ()
	    end
	    function self:tile_collide_hook ()
		if t.sparks then
		    spawn_sparks_on_clients (self.x, self.y, t.sparks, 2)
		end
		self:set_stale ()
	    end	
	end
    })
end


-- Declare a new projectile type.  This handles exploding projectiles.
local Explosive_Projectile = function (t)
    return Objtype (t, {
	nonproxy_init = function (self)
	    object_set_collision_is_projectile (self)
	    if t.dot_mask then
		self:set_mask (mask_main,   "/basic/weapon/single-dot-mask", 0, 0)
		self:set_mask (mask_top,    "/basic/weapon/single-dot-mask", 0, 0)
		self:set_mask (mask_bottom, "/basic/weapon/single-dot-mask", 0, 0)
		self:set_mask (mask_left,   "/basic/weapon/single-dot-mask", 0, 0)
		self:set_mask (mask_right,  "/basic/weapon/single-dot-mask", 0, 0)
	    end
	    self.damage = t.damage
	    local hook = function (self, obj)
		if t.explosion then
		    spawn_explosion_on_clients (t.explosion, self.x, self.y)
		end
		if t.sparks then
		    spawn_sparks_on_clients (self.x, self.y, t.sparks, 2)
		end
		spawn_blast (self.x, self.y, t.radius, self.damage, self.owner)
		self:set_stale ()
	    end
	    self.collide_hook = hook
	    self.tile_collide_hook = hook
	end,

	proxy_init = function (self)
	    self:add_light ("/basic/light/white-16", 0, 0)

            if t.proxy_init then
		t.proxy_init (self)
	    end

	    if t.smoke_trails then
		self:set_update_hook (
		    80,
		    function (self)
			spawn_explosion (t.smoke_trails, self.x, self.y)
		    end
		)
	    end
	end,
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
	local proj = spawn_projectile ("basic-blaster-projectile", player, 10)
	player:set_fire_delay (0.1)
	return proj
    end,
    sound = "/basic/weapon/blaster/sound"
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
    damage = 8,
    sparks = 50,
    dot_mask = true,
    proxy_init = function (self)
	self:move_layer (0, 3, 0)
	self:rotate_layer (0, radian_to_bangle (self.angle))
    end
}


----------------------------------------------------------------------
--  Bow and arrow
----------------------------------------------------------------------

Weapon {
    name = "basic-bow",
    ammo_type = "basic-arrow",
    fire = function (player)
	local proj = spawn_projectile ("basic-arrow-projectile", player, 12, 
				   ((math.random(10) - 5) / 10) * (math.pi/48))
	player:set_fire_delay (0.3)
	player:deduct_ammo ("basic-arrow")
	return proj
    end,
    arm_anim = {
	"/basic/weapon/bow/2arm000",
	"/basic/weapon/bow/2arm001",
	"/basic/weapon/bow/2arm002",
	"/basic/weapon/bow/2arm003",
	"/basic/weapon/bow/2arm004",
	"/basic/weapon/bow/2arm005";
	cx = 4, cy = 5, tics = 3
    },
    sound = "/basic/weapon/bow/sound"
}

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

Explosive_Projectile {
    name = "basic-arrow-projectile",
    alias = "~ap",
    icon = "/basic/weapon/bow/projectile",
    radius = 55,
    damage = 40,
    explosion = "basic-explo42",
    dot_mask = true,
    proxy_init = function (self)
	self:move_layer (0, 8, 1)
	self:rotate_layer (0, radian_to_bangle (self.angle))
    end
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
	"/basic/weapon/ak/2arm004";
	cx = 0, cy = 3
    },
    sound = "/basic/weapon/ak/sound"
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
    damage = 12,
    sparks = 60
}

Weapon {
    name = "basic-minigun",
    ammo_type = "basic-bullet",
    fire = function (player)
	local proj = spawn_projectile ("basic-minigun-projectile", player, 12,
				       ((math.random(10) - 5) / 10) * (math.pi/48))
	player:set_fire_delay (0.06)
	player:deduct_ammo ("basic-bullet")
	return proj
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
    },
    sound = "/basic/weapon/minigun/sound"
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
    damage = 10,
    sparks = 70
}


----------------------------------------------------------------------
--  Rocket weapons
----------------------------------------------------------------------

Weapon_With_Firer {
    name = "basic-rpg",
    ammo_type = "basic-rocket",
    projectile = "basic-rocket-projectile",
    projectile_speed = 10,
    fire_delay_secs = 0.6,
    arm_anim = {
	"/basic/weapon/rpg/2arm000",
	"/basic/weapon/rpg/2arm001",
	"/basic/weapon/rpg/2arm002",
	"/basic/weapon/rpg/2arm003",
	"/basic/weapon/rpg/2arm004",
	"/basic/weapon/rpg/2arm005";
	cx = 13, cy = 3, tics = 3
    },
    sound = "/basic/weapon/rpg/sound"
}

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
    ammo_amount = 5,
    respawn_secs = 10,
}

Explosive_Projectile {
    name = "basic-rocket-projectile",
    alias = "~rp",
    icon = "/basic/weapon/rpg/projectile",
    radius = 75,
    damage = 80,
    dot_mask = true,
    explosion = "basic-explo42",
    smoke_trails = "basic-rocket-smoke",
    proxy_init = function (self)
	self:move_layer (0, 5, 3)
	self:rotate_layer (0, radian_to_bangle (self.angle))
    end
}

explosion_type_register (
    "basic-rocket-smoke",
    "/basic/weapon/rpg/smoke/000", 16, ticks_per_second/16,
    nil, -- no light
    nil  -- no sound
)


----------------------------------------------------------------------
--  Shotgun
----------------------------------------------------------------------

Weapon {
    name = "basic-shotgun",
    ammo_type = "basic-shell",
    fire = function (player)
	local spread = math.pi / 96
	local proj = {
	    spawn_projectile ("basic-shotgun-projectile", player, 10, -2 * spread),
	    spawn_projectile ("basic-shotgun-projectile", player, 10, -spread),
	    spawn_projectile ("basic-shotgun-projectile", player, 10, 0),
	    spawn_projectile ("basic-shotgun-projectile", player, 10, spread),
	    spawn_projectile ("basic-shotgun-projectile", player, 10, 2 * spread)
	}
	player:set_fire_delay (0.4)
	player:deduct_ammo ("basic-shell")
	return proj
    end,
    arm_anim = {
	"/basic/weapon/shotgun/2arm000",
	"/basic/weapon/shotgun/2arm001",
	"/basic/weapon/shotgun/2arm002",
	"/basic/weapon/shotgun/2arm003";
	cx = 1, cy = 4, tics = 4
    },
    sound = "/basic/weapon/shotgun/sound"
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
    damage = 10,
    sparks = 30
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
	_internal_set_camera (true, 300)
    end,
    sound = "/basic/weapon/rifle/sound"
}

Standard_Pickup {
    name = "basic-rifle",
    icon = "/basic/weapon/rifle/pickup",
    weapon_to_give = "basic-rifle",
    ammo_to_give = "basic-slug",
    ammo_amount = 5,
    respawn_secs = 10
}

Objtype {
    name = "basic-rifle-projectile",
    alias = "~Rp",
    icon = "/basic/weapon/shotgun/projectile", -- XXX
    nonproxy_init = function (self)
	self.damage = 50
	function self:collide_hook (obj)
	    obj:receive_damage (self.damage, self.owner, self.x, self.y)
	    self:set_stale ()
	end
	function self:tile_collide_hook (obj)
	    -- sniper rifle slugs don't collide with tiles, but for fun
	    -- we make them spawn sparks the first time they hit one 
	    spawn_sparks_on_clients (self.x, self.y, 50, 2)
	    self:set_collision_flags ("pn")
	    self.tile_collide_hook = nil
	    return false
	end
    end
}


----------------------------------------------------------------------
--  Vulcan cannon
----------------------------------------------------------------------

Standard_Pickup {
    name = "basic-vulcan-ammo",
    icon = "/basic/weapon/ammo/vulcan",
    ammo_to_give = "basic-vulcan-ammo",
    ammo_amount = 20,
    respawn_secs = 10
}

Weapon_With_Firer {
    name = "basic-vulcan",
    ammo_type = "basic-vulcan-ammo",
    projectile = "basic-vulcan-projectile",
    projectile_speed = 300,
    fire_delay_secs = 0.06,
    arm_anim = {
	"/basic/weapon/vulcan/2arm000",
	"/basic/weapon/vulcan/2arm001",
	"/basic/weapon/vulcan/2arm002",
	"/basic/weapon/vulcan/2arm003",
	"/basic/weapon/vulcan/2arm004";
	cx = 0, cy = 3, tics = 2
    },
    sound = "/basic/weapon/vulcan/sound"
}

Standard_Pickup {
    name = "basic-vulcan",
    icon = "/basic/weapon/vulcan/pickup",
    weapon_to_give = "basic-vulcan",
    ammo_to_give = "basic-vulcan-ammo",
    ammo_amount = 10,
    respawn_secs = 10
}

Standard_Projectile {
    name = "basic-vulcan-projectile",
    alias = "~vp",
    icon = "/basic/weapon/vulcan/projectile",
    damage = 7,
    sparks = 60,
}


----------------------------------------------------------------------
--  Mines
----------------------------------------------------------------------

Respawning_Item {
    category = "weapon",
    name = "basic-mine",
    icon = "/basic/weapon/mine/pickup",
    respawn_secs = 60,
    collide_hook = function (self, player)
	player:receive_mines (1)
	play_sound_on_clients (self, "/basic/weapon/generic-pickup-sound")
    end
}

local mine_anim = {
    "/basic/weapon/mine/mine000",
    "/basic/weapon/mine/mine001",
    "/basic/weapon/mine/mine002",
    "/basic/weapon/mine/mine003"
}

local mine_tag = new_object_collision_tag ()

Objtype {
    name = "basic-mine-dropped",
    alias = "~Md",
    icon = "/basic/weapon/mine/mine000",

    nonproxy_init = function (self)
	self.mass = 0.005

	-- Initially the mine doesn't touch players..
	self:set_collision_flags ("tn")
	self:set_mask (mask_bottom, "/basic/weapon/mine/dropping-mask", 0, 0)

	-- It doesn't touch other mines either
	object_set_collision_tag (self, mine_tag)

	self:set_update_hook (
	    1000,
	    function (self)
		self:remove_update_hook ()

		-- Later the mine can touch players.
		-- It also gets a larger mask.
		self:set_collision_flags ("tnp")
		self:set_mask (mask_main, "/basic/weapon/mine/mask", 8, 9)

		function self:collide_hook (obj)
		    if obj.is_player then -- don't explode if only touching crates
			self:die ()
		    end
		end

		function self:receive_damage (obj)
		    -- called if hit by a projectile or blast
		    self:die ()
		end

		function self:die ()
		    -- spew some ball bearings, a blast and play a sound
		    for i, deg in { -10, -30, -50, -70, -90, -110, -130, -150, -170} do
			spawn_projectile_raw ("basic-mine-projectiles",
					      self.owner, self.x, self.y,
					      math.rad (deg), 5)
		    end
		    spawn_blast (self.x, self.y, 25, 20, self.owner)
		    spawn_explosion_on_clients ("basic-explo20", self.x, self.y-6)
		    self:set_stale ()
		end
	    end
	)
    end,

    proxy_init = function (self)
	self.frame = 1

	self.light_tics = 0
	self.have_light = false
	self.light_colour = 1

	self:set_update_hook (
	    250,
	    function (self)
		-- animation
		self.frame = self.frame + 1
		if self.frame > table.getn (mine_anim) then
		    self.frame = 1
		end
		self:replace_layer (0, mine_anim[self.frame], 3, 1)

		-- blinkenlights
		self.light_tics = self.light_tics - 1
		if self.light_tics <= 0 then
		    self.light_tics = 3

		    if self.have_light then
			self:remove_all_lights ()
			self.have_light = false
		    else
			if self.light_colour == 1 then
			    self:add_light ("/basic/light/brown-16", 0, 0)
			    self.light_colour = 2
			else
			    self:add_light ("/basic/light/violet-16", 0, 0)
			    self.light_colour = 1
			end
			self.have_light = true
		    end
		end
	    end
	)
    end
}

Standard_Projectile {
    name = "basic-mine-projectiles",
    alias = "~Mp",
    icon = "/basic/weapon/shotgun/projectile", -- XXX
    damage = 10
}


----------------------------------------------------------------------
--  Weapon switch order
----------------------------------------------------------------------

-- This is the order that weapons will be numbered for selection.
weapon_order = {
    "basic-blaster",
    "basic-shotgun",
    "basic-ak",
    "basic-vulcan",
    "basic-minigun",
    "basic-bow",
    "basic-rpg",
    "basic-rifle"
}

-- This is the order in which weapons will be auto-selected for you.
-- I recommend not putting in explosives.
weapon_auto_switch_order = {
    "basic-minigun",
    "basic-vulcan",
    "basic-shotgun",
    "basic-ak",
    "basic-blaster"
}
