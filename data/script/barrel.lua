-- barrel.lua

function __module__init ()
    store_load ("object/barrel.dat", "/barrel/")
    object_type_register ("barrel-grey", nil, "objtile", "/barrel/barrel-grey")
    object_type_register ("barrel-red",  nil, "objtile", "/barrel/barrel-red")
end
