-- basic-tilelike.lua

store_load ("basic/basic-tilelike.dat", "/basic/tilelike/")

objtype_register ("objtile", "basic-barrel-red", "/basic/tilelike/barrel000")
objtype_register ("objtile", "basic-barrel-grey", "/basic/tilelike/barrel001")
objtype_register ("objtile", "basic-crate-000", "/basic/tilelike/crate-normal000")
objtype_register ("objtile", "basic-crate-001", "/basic/tilelike/crate-normal001")
objtype_register ("objtile", "basic-crate-large-000", "/basic/tilelike/crate-large000")


----------------------------------------------------------------------
--  Ladders
----------------------------------------------------------------------

Objtype {
    category = "basic-ladder",
    name = "basic-ladder-main",
    icon = "/basic/tilelike/ladder/000",
    nonproxy_init = function (self)
	object_set_collision_is_ladder (self)
    end
}
