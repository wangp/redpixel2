-- basic-powerup.lua

store_load ("basic/basic-powerup.dat", "/basic/powerup/")

function basic_powerup_init (self)
    self:set_collision_flags ("p")
    function self.collide_hook (self, player)
	self:destroy (self)
    end
end

-- enhancements
Objtype {
    category = "item", 
    name = "basic-armour-brown", 
    icon = "/basic/powerup/armour/brown",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item", 
    name = "basic-armour-purple", 
    icon = "/basic/powerup/armour/purple",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item", 
    name = "basic-armour-blue", 
    icon = "/basic/powerup/armour/blue",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item", 
    name = "basic-bloodlust", 
    icon = "/basic/powerup/bloodlust/000",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item", 
    name = "basic-cordial", 
    icon = "/basic/powerup/cordial",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item",
    name = "basic-lightamp", 
    icon = "/basic/powerup/lightamp/000", 
    nonproxy_init = basic_powerup_init,
    proxy_init = function (self)
	self:add_light ("/basic/light/blue-16", 0, 0)
    end
}

-- health restoration
Objtype {
    category = "item",
    name = "basic-burger",
    icon = "/basic/powerup/burger",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item", 
    name = "basic-chocolate", 
    icon = "/basic/powerup/chocolate",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item", 
    name = "basic-joltcola", 
    icon = "/basic/powerup/joltcola",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item", 
    name = "basic-medikit", 
    icon = "/basic/powerup/medikit",
    nonproxy_init = basic_powerup_init
}

Objtype {
    category = "item", 
    name = "basic-pizza", 
    icon = "/basic/powerup/pizza",
    nonproxy_init = basic_powerup_init
}
