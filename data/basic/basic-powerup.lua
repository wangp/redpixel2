-- basic-powerup.lua

store_load ("basic/basic-powerup.dat", "/basic/powerup/")

-- enhancements
objtype_register ("item", "basic-armour-brown", "/basic/powerup/armour/brown")
objtype_register ("item", "basic-armour-purple", "/basic/powerup/armour/purple")
objtype_register ("item", "basic-armour-blue", "/basic/powerup/armour/blue")
objtype_register ("item", "basic-bloodlust", "/basic/powerup/bloodlust/000")
objtype_register ("item", "basic-cordial", "/basic/powerup/cordial")
objtype_register ("item", "basic-lightamp", "/basic/powerup/lightamp/000", 
			function (self)
			    if self.is_proxy then
				self:add_light ("/basic/light/blue-16", 0, 0)
			    end
			end)

-- health restoration
objtype_register ("item", "basic-burger", "/basic/powerup/burger")
objtype_register ("item", "basic-chocolate", "/basic/powerup/chocolate")
objtype_register ("item", "basic-joltcola", "/basic/powerup/joltcola")
objtype_register ("item", "basic-medikit", "/basic/powerup/medikit")
objtype_register ("item", "basic-pizza", "/basic/powerup/pizza")
