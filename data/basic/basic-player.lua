-- basic-player.lua

store_load ("basic/basic-player.dat", "/basic/player/")


----------------------------------------------------------------------
--  Players
----------------------------------------------------------------------

local xv_decay, yv_decay = 0.75, 0.5

Objtype {
    category = "player",    -- XXX: should be nil in future; the map editor should not allow it to be placed
    name = "player",	    -- not basic-player
    icon = "/basic/player/walk/000",


    --
    -- Non-proxy
    --
    nonproxy_init = function (self)
	local cx, cy = 5, 5

	self.xv_decay = %xv_decay
	self.yv_decay = %yv_decay
	self._ramp = 6
	self.mass = 0.9

	object_set_collision_is_player (self)
	self:set_collision_flags ("tn")
	self:set_mask (mask_main, "/basic/player/mask/whole", cx, cy)
	self:set_mask (mask_top, "/basic/player/mask/top", cx, cy)
	self:set_mask (mask_bottom, "/basic/player/mask/bottom", cx, cy)
	self:set_mask (mask_left, "/basic/player/mask/left", cx, cy)
	self:set_mask (mask_right, "/basic/player/mask/right", cx, cy)

	self.health = 100

	self.fire_delay = 0

	-- XXX: temp
	self.current_weapon = weapons["basic-blaster"]
	
	function self._fire_hook (self)
	    if self.fire_delay <= 0 then
		local w = self.current_weapon
		if w and w.can_fire (self) then
		    w.fire (self)
		end
	    end
	end

	self:set_update_hook (
	    1000/50,
	    function (self)
		self.fire_delay = self.fire_delay - 1
	    end
	)

	function self.receive_weapon (self, weapon_name)
	    if weapons[weapon_name] then
		self.have_weapon[weapon_name] = 1
		if not self.current_weapon then
		    self.current_weapon = weapon[weapon_name]
		end
	    end
	end

	function self.receive_damage (self, damage)
	    self.health = self.health - damage
	    if self.health <= 0 then
		spawn_object ("basic-player-death-fountain", self.x, self.y)
		self:destroy ()
	    end
	    spawn_blood (self.x + %cx, self.y + %cy, 100, 2000)
	    -- XXX spread should be float
	end
    end,


    --
    -- Proxy
    --
    proxy_init = function (self)
	local cx, cy = 5, 5

	self.xv_decay = %xv_decay
	self.yv_decay = %yv_decay

	self:move_layer (0, cx, cy)
	self.arm_layer = self:add_layer ("/basic/weapon/blaster/1arm000", 0, 3)
	
	if not self.is_local then
	    self:add_light ("/basic/light/white-32", 0, 0)
	else
	    self:add_light ("/basic/light/white-64", 0, 0)

	    self:set_update_hook (
		1000/50,
		function (self)
		    if self.last_aim_angle ~= self.aim_angle then
			local a = radian_to_bangle (self.aim_angle)
			local hflipped = (a < -63 or a > 63) or 0
			
			self:hflip_layer (0, hflipped)
			self:hflip_layer (self.arm_layer, hflipped)
			
			if hflipped ~= 0
			then self:rotate_layer (self.arm_layer, a-128)
			else self:rotate_layer (self.arm_layer, a)
			end

			self.last_aim_angle = self.aim_angle
		    end
		end
	    )
	end
    end
}


----------------------------------------------------------------------
--  Corpses
----------------------------------------------------------------------

Objtype {
    category = "player",
    name = "basic-player-death-fountain",
    icon = "/basic/player/death-fountain/000",

    nonproxy_init = function (self)
	self:set_collision_flags ("")
    end,

    proxy_init = function (self)
	self:replace_layer (0, "/basic/player/death-fountain/000", 44, 16)
	self.frame = 1
	self:set_update_hook (
	    1000/10, 
	    function (self)
		self:replace_layer (0, format ("/basic/player/death-fountain/%03d", self.frame), 44, 16)
		self.frame = self.frame + 1
		if self.frame > 30 then
		    self:remove_update_hook ()
		end
	    end
	)
    end
}
