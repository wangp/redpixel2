-- basic-player.lua

store_load ("basic/basic-player.dat", "/basic/player/")

local xv_decay, yv_decay = 0.75, 0.75

Objtype {
    category = "player",    -- XXX: should be nil in future
    name = "player",	    -- not basic-player
    icon = "/basic/player/walk/000",

    nonproxy_init = function (self)
	local cx, cy = 5, 5

	self.xv_decay = %xv_decay
	self.yv_decay = %yv_decay
	self._ramp = 6
	self.mass = 2.2

	object_set_collision_is_player (self)
	self:set_collision_flags ("tn")
	self:set_mask (mask_main, "/basic/player/mask/whole", cx, cy)
	self:set_mask (mask_top, "/basic/player/mask/top", cx, cy)
	self:set_mask (mask_bottom, "/basic/player/mask/bottom", cx, cy)
	self:set_mask (mask_left, "/basic/player/mask/left", cx, cy)
	self:set_mask (mask_right, "/basic/player/mask/right", cx, cy)

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

	function self._client_update_hook (self)
	    self.fire_delay = self.fire_delay - 1
	end

	function self.receive_weapon (self, weapon_name)
	    if not weapons[weapon_name] then return end
	    self.have_weapon[weapon_name] = 1
	    if not self.current_weapon then
	    	self.current_weapon = weapon[weapon_name]
	    end
	end
    end,

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

	    function self._client_update_hook (self)
		function radian_to_bangle (rads) return rads * 128 / 3.1415 end
		if self.last_aim_angle ~= self.aim_angle then
		    self:rotate_layer (self.arm_layer, radian_to_bangle (self.aim_angle))
		    self.last_aim_angle = self.aim_angle
		end
	    end
	end
    end
}
