-- lightamp.lua

local frames = {
    "/tv/anim1", 
    "/tv/anim2",
    "/tv/anim3", 
}

local tv = {
    init = function (self)
	self.animframe = 1
	self.animtics = 60/10
    end,
    
    process = function (self)
	self.animtics = self.animtics - 1
    
	if self.animtics < 1 then
	    set_visual (self, %frames[self.animframe])
	    self.animframe = self.animframe + 1 
	    if self.animframe > 3 then
		self.animframe = 1
	    end

	    self.animtics = 60/10
	end
    end
}

function __module__init ()
    store_load ("object/tv.dat", "/tv/")
    register_object ("tv", %tv, "item", "/tv/anim1")
end
