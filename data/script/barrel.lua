-- barrel.lua

local barrel = {
}

function __module__init ()
    store_load ("object/barrel.dat", "/barrel/")
    register_object ("barrel-grey", %barrel, "objtile", "/barrel/barrel-grey")
    register_object ("barrel-red",  %barrel, "objtile", "/barrel/barrel-red")
end
