-- player.lua

local player = {
    init = function (self)
	self.rep = {
	    "/player/body",
	    { "/player/legs-walk-1"; offsetx = -2, offsety = 9 }
	}
	self.n = 1

	set_visual_layers (self, self.rep)
    end,

    process = function (self)
	local list = {
	    "/player/legs-walk-1",
	    "/player/legs-walk-2",
	    "/player/legs-walk-3",
	    "/player/legs-walk-4",
	    "/player/legs-walk-5",
	    "/player/legs-walk-6",
	    "/player/legs-walk-7",
	    "/player/legs-walk-8",
	    "/player/legs-walk-9"
	}

	self.n = self.n + 1
	if self.n > 9 then self.n = 1 end

	self.rep[2][1] = list[self.n]
	set_visual_layers (self, self.rep)
    end
}


function __module__init ()
    store_load ("object/player.dat", "/player/")
    register_object ("player", %player, "player", "/player/body")
end
