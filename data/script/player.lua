-- player.lua

local player = {
    init = function (self)
	self.rep = {
	    "/player/body",
	    { "/player/legs-walk-1"; offsetx = -2, offsety = 9 }
	}
	self.n = 1
	self.anim = 0

	object_set_visual_image (self, self.rep)
	object_set_collision_masks (self, "/player/mask",
			     		"/player/mask-top",
			     		"/player/mask-bottom",
			     		"/player/mask-left",
					"/player/mask-right")

	self._jump = 0
    end,

    process = function (self)
	if self.role == "authority" then return end

	self.anim = self.anim + 1
	if self.anim < 3 then return end
	self.anim = 0

	local list = {
	    "/player/legs-walk-0",
	    "/player/legs-walk-1",
	    "/player/legs-walk-2",
	    "/player/legs-walk-3",
	    "/player/legs-walk-4",
	    "/player/legs-walk-5",
	    "/player/legs-walk-6",
	    "/player/legs-walk-7"
	}

	self.n = self.n + 1
	if self.n >= 9 then self.n = 1 end

	self.rep[2][1] = list[self.n]
	object_set_visual_image (self, self.rep)
    end
}


function __module__init ()
    store_load ("object/player.dat", "/player/")
    object_type_register ("player", %player, "player", "/player/body")
end
