-- lightamp.lua

local frames = {
    "/lightamp/anim1", 
    "/lightamp/anim2",
    "/lightamp/anim3", 
    "/lightamp/anim4",
    "/lightamp/anim3", 
    "/lightamp/anim2", 
    "/lightamp/anim1"
}

local lightamp = {
    init = function (self)
	self.animframe = 1
	self.animtics = 60/10
    end,
    
    process = function (self)
	self.animtics = self.animtics - 1
    
	if self.animtics < 1 then
	    set_visual (self, %frames[self.animframe])
	    self.animframe = self.animframe + 1 
	    if self.animframe > 7 then
		self.animframe = 1
	    end

	    self.animtics = 60/10
	end
    end
}

function __module__init ()
    store_load ("object/lightamp.dat", "/lightamp/")
    register_object ("lightamp", %lightamp, "item", "/lightamp/anim1")
end
