-- basic-tilelike.lua

store_load ("basic/basic-tilelike.dat", "/basic/tilelike/")

objtype_register ("objtile", "basic-barrel-red", "/basic/tilelike/barrel-red/main")
objtype_register ("objtile", "basic-barrel-grey", "/basic/tilelike/barrel-gray/main")



----------------------------------------------------------------------
--  Crates
----------------------------------------------------------------------

local make_crate_nonproxy_init
function make_crate_nonproxy_init (health, respawn_secs)
    return function (self)
	self.health = health
	function self:receive_damage (amount)
	    self.health = self.health - amount
	    if self.health <= 0 then
		self:hide_and_respawn_later (respawn_secs * 1000)
	    end
	end
    end
end

Objtype {
    category = "objtile",
    name = "basic-crate-000",
    icon = "/basic/tilelike/crate-normal000",
    nonproxy_init = make_crate_nonproxy_init (15, 20)
}

Objtype {
    category = "objtile",
    name = "basic-crate-001",
    icon = "/basic/tilelike/crate-normal001",
    nonproxy_init = make_crate_nonproxy_init (15, 20)
}

Objtype {
    category = "objtile",
    name = "basic-crate-large-000",
    icon = "/basic/tilelike/crate-large000",
    nonproxy_init = make_crate_nonproxy_init (30, 20)
}


----------------------------------------------------------------------
--  Ladders
----------------------------------------------------------------------

local Ladder = function (t)
    return Objtype (t, {
	category = "basic-ladder",
	nonproxy_init = function (self)
	    object_set_collision_is_ladder (self)
	    self:set_mask (mask_main, t.mask, t.cx, t.cy)
	end
    })
end

Ladder {
    name = "basic-ladder-gray1",
    icon = "/basic/tilelike/ladder-gray1",
    mask = "/basic/tilelike/ladder-gray1-mask",
    cx = 8, cy = 8
}

Ladder {
    name = "basic-ladder-gray3",
    icon = "/basic/tilelike/ladder-gray3",
    mask = "/basic/tilelike/ladder-gray3-mask",
    cx = 8, cy = 24
}

Ladder {
    name = "basic-ladder-gray5",
    icon = "/basic/tilelike/ladder-gray5",
    mask = "/basic/tilelike/ladder-gray5-mask",
    cx = 8, cy = 40
}
