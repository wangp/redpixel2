-- enhance.lua

function init_lightamp (self)
    self:add_light ("/lights/blue-32", 0, 0)
    self.mass = 0
end


-- Module init.

store_load ("object/enhance.dat", "/enhancements/")
objtype_register ("item", "armour-brown",	"/enhancements/armour-000")
objtype_register ("item", "armour-purple",	"/enhancements/armour-001")
objtype_register ("item", "armour-blue",	"/enhancements/armour-002")
objtype_register ("item", "bloodlust",		"/enhancements/bloodlust-000")
objtype_register ("item", "cordial",		"/enhancements/cordial")
objtype_register ("item", "lightamp",		"/enhancements/lightamp-000", init_lightamp)
