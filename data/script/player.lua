-- player.lua

function init_player (self)
    self.mass = 2.2
    self.ramp = 6
    self:add_light ("/lights/white-64", 0, 0)
    self:set_mask (0, "/player/mask-whole", -3, -8)
    self:set_mask (1, "/player/mask-top", -3, -8)
    self:set_mask (2, "/player/mask-bottom", -3, -8)
    self:set_mask (3, "/player/mask-left", -3, -8)
    self:set_mask (4, "/player/mask-right", -3, -8)

    self:move_layer (0, -1, -4)
    self.walk_frame = 0
    self.leg_layer = self:add_layer ("/player/legs-walk-0", 0, 4)

    -- XXX
    self.arm_frame = 0
    self.arm_layer = self:add_layer ("/weapon/shotgun/2arm-000", 7, -4)

    self.walk_hook = function (self)
	if self.walk_frame < 7
	then self.walk_frame = self.walk_frame + 1
	else self.walk_frame = 0 
	end
	self:replace_layer (self.leg_layer, "/player/legs-walk-"..self.walk_frame, 0, 4)

	if self.arm_frame < 4
	then self.arm_frame = self.arm_frame + 1
	else self.arm_frame = 0 
	end
	self:replace_layer (self.arm_layer, "/weapon/shotgun/2arm-00"..self.arm_frame, 7, -4)
    end

end

-- Module init.

store_load ("object/player.dat", "/player/")
store_load ("object/pllight.dat", "/player/")
objtype_register ("player", "player", "/player/body", init_player)
