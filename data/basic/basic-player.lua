-- basic-player.lua

store_load ("basic/basic-player.dat", "/basic/player/")

-- XXX shouldn't have to do this
function radian_to_bangle (rads)
    return rads * 128 / 3.1415
end

function basic_player_init (self)
    local cx, cy = 5, 5

    if not self.is_proxy then
	self.mass = 0.5		-- XXX to point out a bug
--	self.mass = 2.2
	self.ramp = 6
	self:set_mask (mask_main, "/basic/player/mask/whole", cx, cy)
	self:set_mask (mask_top, "/basic/player/mask/top", cx, cy)
	self:set_mask (mask_bottom, "/basic/player/mask/bottom", cx, cy)
	self:set_mask (mask_left, "/basic/player/mask/left", cx, cy)
	self:set_mask (mask_right, "/basic/player/mask/right", cx, cy)
    else
	self:move_layer (0, cx, cy)
	self.arm_layer = self:add_layer ("/basic/weapon/blaster/1arm000", 0, 3)
--	self.arm_layer = self:add_layer ("/basic/weapon/minigun/2arm000", 0, 0)
	if not self.is_client then
	    self:add_light ("/basic/light/white-32", 0, 0)
	else
	    self:add_light ("/basic/light/white-64", 0, 0)

	    function self.client_update (self)
		if self.last_aim_angle ~= self.aim_angle then
		    self:rotate_layer (self.arm_layer, radian_to_bangle (self.aim_angle))
		    self.last_aim_angle = self.aim_angle
		end
	    end
	end
    end
end

-- Note: object type name should NOT be basic-player
objtype_register ("player", "player", "/basic/player/walk/000", basic_player_init)
