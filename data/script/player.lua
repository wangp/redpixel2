-- player.lua

function init_player (self)
    object_add_light (self, "/lights/white-64", 0, 0)

    object_move_layer (self, 0, -1, -4)

    object_set_collision_mask (self, "/player/mask-whole", -3, -8)
    
    self.walk_frame = 0
    self.leg_layer = 
    object_add_layer (self, "/player/legs-walk-0", 0, 4)

    self.walk_hook = function (self)
	if self.walk_frame < 7
	then self.walk_frame = self.walk_frame + 1
	else self.walk_frame = 0 
	end
	object_replace_layer 
	(self, self.leg_layer, "/player/legs-walk-" .. self.walk_frame, 0, 4)
    end
end

-- Module init.

store_load ("object/player.dat", "/player/")
store_load ("object/pllight.dat", "/player/")
objtype_register ("player", "player", "/player/body", init_player)
