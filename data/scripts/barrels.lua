-- barrels.lua

local barrel = {
    spawn = function ()
    end
}

function __module__init ()
    store_load ("objects/barrels.dat", "/barrels/")
    register_object (%barrel, "objtiles", "/barrels/greybarrel")
    register_object (%barrel, "objtiles", "/barrels/redbarrel")
end
