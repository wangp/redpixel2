-- basic-powerup.lua

store_load ("basic/basic-powerup.dat", "/basic/powerup/")


local Item = Respawning_Item


----------------------------------------------------------------------
-- Enhancements
----------------------------------------------------------------------


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

local lightamp_anim = {
    "/basic/powerup/lightamp/000",
    "/basic/powerup/lightamp/000",
    "/basic/powerup/lightamp/001",
    "/basic/powerup/lightamp/002",
    "/basic/powerup/lightamp/003",
    "/basic/powerup/lightamp/002",
    "/basic/powerup/lightamp/001"
}

Item {
    name = "basic-lightamp", 
    icon = "/basic/powerup/lightamp/000", 
    respawn_secs = 10,
    collide_hook = function (self, player)
	call_method_on_clients (player, "start_alt_light",
	    "return '/basic/powerup/lightamp/light', 3000, 20")
    end,
    proxy_init = function (self)
	self:add_light ("/basic/light/blue-16", 0, 0)

	self.frame = 1
	self:set_update_hook (
	    250 + random (50),
	    function (self)
		self.frame = self.frame + 1
		if self.frame > getn (lightamp_anim) then
		    self.frame = 1
		end
		self:replace_layer (0, lightamp_anim[self.frame], 7, 3)
	    end
	)
    end
}


----------------------------------------------------------------------
--  Health restoration
----------------------------------------------------------------------

local Health_Item = function (t)
    return Item (t, {
	collide_hook = function (self, player)
	    local h = player.health
	    player:receive_health (t.give_health)
	    if player.health == h then
		return false
	    end
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
