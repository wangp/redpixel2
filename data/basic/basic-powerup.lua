-- basic-powerup.lua

store_load ("basic/basic-powerup.dat", "/basic/powerup/")

local make_basic_powerup_init = function (respawn_secs)
    return function (self)
	self:set_collision_flags ("p")
	function self:collide_hook (player)
	    self:hide ()
	    self:set_update_hook (
		1000 * respawn_secs,
		function (self)
		    self:show ()
		    self:remove_update_hook ()
		end
	    )
	end
    end
end

-- enhancements
Objtype {
    category = "item", 
    name = "basic-armour-brown", 
    icon = "/basic/powerup/armour/brown",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item", 
    name = "basic-armour-purple", 
    icon = "/basic/powerup/armour/purple",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item", 
    name = "basic-armour-blue", 
    icon = "/basic/powerup/armour/blue",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item", 
    name = "basic-bloodlust", 
    icon = "/basic/powerup/bloodlust/000",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item", 
    name = "basic-cordial", 
    icon = "/basic/powerup/cordial",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item",
    name = "basic-lightamp", 
    icon = "/basic/powerup/lightamp/000", 
    nonproxy_init = make_basic_powerup_init (10),
    proxy_init = function (self)
	self:add_light ("/basic/light/blue-16", 0, 0)
    end
}

-- health restoration
Objtype {
    category = "item",
    name = "basic-burger",
    icon = "/basic/powerup/burger",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item", 
    name = "basic-chocolate", 
    icon = "/basic/powerup/chocolate",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item", 
    name = "basic-joltcola", 
    icon = "/basic/powerup/joltcola",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item", 
    name = "basic-medikit", 
    icon = "/basic/powerup/medikit",
    nonproxy_init = make_basic_powerup_init (10)
}

Objtype {
    category = "item", 
    name = "basic-pizza", 
    icon = "/basic/powerup/pizza",
    nonproxy_init = make_basic_powerup_init (10)
}
