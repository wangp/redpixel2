-- basic-tilelike.lua

store_load ("basic/basic-tilelike.dat", "/basic/tilelike/")


----------------------------------------------------------------------
--  Crates
----------------------------------------------------------------------

local Crate = function (t)
    return Objtype (t, {
	category = "objtile",
	nonproxy_init = function (self)
	    self.health = t.health
	    function self:receive_damage (amount)
		self.health = self.health - amount
		if self.health <= 0 then
		    spawn_explosion ("basic-explo42", self.x, self.y)
		    spawn_sparks (self.x, self.y, 30, 3)
		    self:hide ()
		    function the_hook (self)
			if _internal_would_collide_with_objects (self) then
			    -- try again later
			    self:set_update_hook (700, the_hook)
			else
			    self:show ()
			    self:remove_update_hook ()
			end
		    end
		    self:set_update_hook (t.respawn_secs * 1000, the_hook)
		end
	    end
	end
    })
end

Crate {
    name = "basic-crate-000",
    icon = "/basic/tilelike/crate-normal000",
    health = 15,
    respawn_secs = 20
}

Crate {
    name = "basic-crate-001",
    icon = "/basic/tilelike/crate-normal001",
    health = 15,
    respawn_secs = 20
}

Crate {
    name = "basic-crate-large-000",
    icon = "/basic/tilelike/crate-large000",
    health = 30,
    respawn_secs = 20
}


----------------------------------------------------------------------
--  Barrels
----------------------------------------------------------------------

local Barrel = function (t)
    return Objtype (t, {
	category = "objtile",
	nonproxy_init = function (self)
	    self.health = t.health
	    function self:receive_damage (amount)
		self.health = self.health - amount
		if self.health <= 0 then
		    spawn_explosion ("basic-explo42", self.x, self.y)
		    spawn_sparks (self.x, self.y, 30, 3)
		    spawn_blast (self.x, self.y, 45, 50)

		    if t.chunks then
			for i = 1, random (10) do
			    spawn_object (t.chunks,
					  self.x + random (17) - 8,
					  self.y + random (17) - 8)
			end
		    end

		    self:hide ()
		    function the_hook (self)
			if _internal_would_collide_with_objects (self) then
			    -- try again later
			    self:set_update_hook (700, the_hook)
			else
			    self:show ()
			    self:remove_update_hook ()
			end
		    end
		    self:set_update_hook (t.respawn_secs * 1000, the_hook)
		end
	    end
	end
    })
end

local BarrelChunks = function (t)
    return Objtype (t, {
	icon = t.images[1],
	nonproxy_init = function (self)
	    self.mass = 0.001
	    self:set_collision_flags ("t")
	    self:set_update_hook (1000 + random (3000), object_destroy)
	end,
	proxy_init = function (self)
	    self:replace_layer (0, t.images[random (getn (t.images))], 2, 2)
	end
    })
end

Barrel {
    name = "basic-barrel-red",
    icon = "/basic/tilelike/barrel-red/main",
    health = 30,
    respawn_secs = 20,
    chunks = "basic-barrel-red-chunks"
}

Barrel {
    name = "basic-barrel-grey",
    icon = "/basic/tilelike/barrel-gray/main",
    health = 30,
    respawn_secs = 20,
    chunks = "basic-barrel-gray-chunks"
}

BarrelChunks {
    name = "basic-barrel-red-chunks",
    images = {
	"/basic/tilelike/barrel-red/chunk0",
	"/basic/tilelike/barrel-red/chunk1",
	"/basic/tilelike/barrel-red/chunk2"
    }
}

BarrelChunks {
    name = "basic-barrel-gray-chunks",
    images = {
	"/basic/tilelike/barrel-gray/chunk0",
	"/basic/tilelike/barrel-gray/chunk1",
	"/basic/tilelike/barrel-gray/chunk2"
    }
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
