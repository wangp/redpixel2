-- off-objs.lua

local monitorfront = {
    init = function (self)
	local anim = {
	    "/office-objects/monitor-anim1",
	    "/office-objects/monitor-anim1",
	    "/office-objects/monitor-anim2", 
	    "/office-objects/monitor-anim3",
	    "/office-objects/monitor-anim4", 
	    "/office-objects/monitor-anim5", 
	    "/office-objects/monitor-anim3"
	}

	object_set_visual_anim (self, anim, 0.25)
    end
}


local tv = {
    init = function (self)
	local tvframes = {
	    "/office-objects/tv-anim1", 
	    "/office-objects/tv-anim2",
	    "/office-objects/tv-anim3", 
	}

	object_set_visual_anim (self, tvframes, 0.10)
    end
}


function __module__init ()
    local register = object_type_register

    store_load ("object/off-objs.dat", "/office-objects/")
    register ("monitor-front", %monitorfront, "objtile", "/office-objects/monitor-anim1")
    register ("monitor-left",  nil,  	      "objtile", "/office-objects/monitor-left")
    register ("monitor-right", nil,  	      "objtile", "/office-objects/monitor-right")
    register ("brain",	       nil,	      "objtile", "/office-objects/brain")
    register ("printer",       nil,	      "objtile", "/office-objects/printer")
    register ("tv",	       %tv, 	      "objtile", "/office-objects/tv-anim1")
end
