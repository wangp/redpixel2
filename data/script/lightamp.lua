-- lightamp.lua

local lightamp = {
}

function __module__init ()
    store_load ("object/lightamp.dat", "/lightamp/")
    register_object ("lightamp", %lightamp, "item", "/lightamp/anim0")
end
