-- basic-tilelike.lua

store_load ("basic/basic-tilelike.dat", "/basic/tilelike/")

objtype_register ("objtile", "basic-barrel-red", "/basic/tilelike/barrel000")
objtype_register ("objtile", "basic-barrel-grey", "/basic/tilelike/barrel001")



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

Objtype {
    category = "basic-ladder",
    name = "basic-ladder-main",
    icon = "/basic/tilelike/ladder/000",
    nonproxy_init = function (self)
	object_set_collision_is_ladder (self)
    end
}
