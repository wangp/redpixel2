-- lightamp.lua

local init_lightmap = function (self)
--     object_set_visual_anim (self, { "/lightamp/anim1",
-- 				    "/lightamp/anim2",
-- 				    "/lightamp/anim3", 
-- 				    "/lightamp/anim4",
-- 				    "/lightamp/anim3", 
-- 				    "/lightamp/anim2", 
-- 				    "/lightamp/anim1" }, 0.1)

    self.player_touched = function (self, player)
-- 	object_destroy (self)
    end
end

-- Module init.

store_load ("object/lightamp.dat", "/lightamp/")
objtype_register ("item", "lightamp", "/lightamp/anim1", init_lightmap)
