-- lightamp.lua

local lightamp = {
    init = function (self)
	local frames = {
	    "/lightamp/anim1", 
	    "/lightamp/anim2",
	    "/lightamp/anim3", 
	    "/lightamp/anim4",
	    "/lightamp/anim3", 
	    "/lightamp/anim2", 
	    "/lightamp/anim1"
	}

	object_set_visual_anim (self, frames, 0.1)
    end,

    player_touched = function (self, player) 
	object_destroy (self)
    end
}

function __module__init ()
    store_load ("object/lightamp.dat", "/lightamp/")
    object_type_register ("lightamp", %lightamp, "item", "/lightamp/anim1")
end
