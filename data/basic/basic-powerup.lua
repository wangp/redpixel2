-- basic-powerup.lua

store_load ("basic/basic-powerup.dat", "/basic/powerup/")

local Item = function (t)
    return Objtype (t, {
	category = "item",
	nonproxy_init = function (self)
	    self:set_collision_flags ("p")
	    function self:collide_hook (player)
		self:hide_and_respawn_later (1000 * t.respawn_secs)
	    end
	end 
    })
end

-- enhancements
Item {
    name = "basic-armour-brown", 
    icon = "/basic/powerup/armour/brown",
    respawn_secs = 10
}

Item {
    name = "basic-armour-purple", 
    icon = "/basic/powerup/armour/purple",
    respawn_secs = 10
}

Item {
    name = "basic-armour-blue", 
    icon = "/basic/powerup/armour/blue",
    respawn_secs = 10
}

Item {
    name = "basic-bloodlust", 
    icon = "/basic/powerup/bloodlust/000",
    respawn_secs = 10
}

Item {
    name = "basic-cordial", 
    icon = "/basic/powerup/cordial",
    respawn_secs = 10
}

Item {
    name = "basic-lightamp", 
    icon = "/basic/powerup/lightamp/000", 
    respawn_secs = 10,
    proxy_init = function (self)
	self:add_light ("/basic/light/blue-16", 0, 0)
    end
}

-- health restoration
Item {
    name = "basic-burger",
    icon = "/basic/powerup/burger",
    respawn_secs = 10
}

Item {
    name = "basic-chocolate", 
    icon = "/basic/powerup/chocolate",
    respawn_secs = 10
}

Item {
    name = "basic-joltcola", 
    icon = "/basic/powerup/joltcola",
    respawn_secs = 10
}

Item {
    name = "basic-medikit", 
    icon = "/basic/powerup/medikit",
    respawn_secs = 10
}

Item {
    name = "basic-pizza", 
    icon = "/basic/powerup/pizza",
    respawn_secs = 10
}
