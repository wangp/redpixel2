-- crate.lua

function __module__init ()
    store_load ("object/crate.dat", "/crate/")
    object_type_register ("crate-1",   nil, "objtile", "/crate/crate-1")
    object_type_register ("crate-2",   nil, "objtile", "/crate/crate-2")
    object_type_register ("crate-big", nil, "objtile", "/crate/crate-big")
end
