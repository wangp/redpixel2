-- off-objs.lua

-- Monitor (front).

local init_monitorfront = function (self)
--     object_set_visual_anim (self, { "/office-objects/monitor-anim1",
-- 				    "/office-objects/monitor-anim1",
-- 				    "/office-objects/monitor-anim2",
-- 			            "/office-objects/monitor-anim3",
-- 				    "/office-objects/monitor-anim4",
-- 				    "/office-objects/monitor-anim5",
-- 				    "/office-objects/monitor-anim3" }, 0.25)
    
    self.player_touched = function (self)
-- 	object_set_visual_bitmap (self, "/office-objects/monitor-blown")
    end
end

-- Television.

local init_tv = function (self)
--     object_set_visual_anim (self, { "/office-objects/tv-anim1", 
-- 				    "/office-objects/tv-anim2",
-- 				    "/office-objects/tv-anim3" }, 0.10)

    self.player_touched = function (self)
-- 	object_set_visual_bitmap (self, "/office-objects/tv-blown")
    end
end

-- Other stuff.

local init_other = function (self, type)
--     local bmp = "/office-objects/" .. type

    self.player_touched = function (self)
-- 	object_set_visual_bitmap (self, %bmp .. "-blown")
    end
end

-- Module.

store_load ("object/off-objs.dat", "/office-objects/")

objtype_register ("objtile", "monitor-front", "/office-objects/monitor-anim1", init_monitorfront)
objtype_register ("objtile", "monitor-left", "/office-objects/monitor-left", init_other)
objtype_register ("objtile", "monitor-right", "/office-objects/monitor-right", init_other)
objtype_register ("objtile", "brain", "/office-objects/brain", init_other)
objtype_register ("objtile", "printer", "/office-objects/printer", init_other)
objtype_register ("objtile", "tv", "/office-objects/tv-anim1", init_tv)
