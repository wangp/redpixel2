-- lightamp.lua

function init_lightamp (self)
    object_add_light (self, "/lights/blue-32", 0, 0)
    object_set_mass (self, 0)
end

-- Module init.

store_load ("object/lightamp.dat", "/lightamp/")
objtype_register ("item", "lightamp", "/lightamp/anim1", init_lightamp)
