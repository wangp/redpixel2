-- basic-powerup.lua

store_load ("basic/basic-powerup.dat", "/basic/powerup/")

local Item = function (t, u)
    t = merge (t, u)
    return Objtype (t, {
	category = "item",
	nonproxy_init = function (self)
	    self:set_collision_flags ("p")
	    function self:collide_hook (player)
		self:hide_and_respawn_later (1000 * t.respawn_secs)
		if t.action then
		    t.action (self, player)
		end
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


----------------------------------------------------------------------
--  Health restoration
----------------------------------------------------------------------

local Health_Item = function (t)
    return Item (t, {
	action = function (self, player)
	    player:receive_health (t.give_health)
	end
    })
end

Health_Item {
    name = "basic-burger",
    icon = "/basic/powerup/burger",
    respawn_secs = 10,
    give_health = 10
}

Health_Item {
    name = "basic-chocolate", 
    icon = "/basic/powerup/chocolate",
    respawn_secs = 10,
    give_health = 5
}

Health_Item {
    name = "basic-joltcola", 
    icon = "/basic/powerup/joltcola",
    respawn_secs = 10,
    give_health = 20
}

Health_Item {
    name = "basic-medikit", 
    icon = "/basic/powerup/medikit",
    respawn_secs = 10,
    give_health = 50
}

Health_Item {
    name = "basic-pizza", 
    icon = "/basic/powerup/pizza",
    respawn_secs = 10,
    give_health = 30
}
