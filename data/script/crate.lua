-- crate.lua

local crate = {
}

function __module__init ()
    store_load ("object/crate.dat", "/crate/")
    register_object ("crate-1",   %crate, "objtile", "/crate/crate-1")
    register_object ("crate-2",   %crate, "objtile", "/crate/crate-2")
    register_object ("crate-big", %crate, "objtile", "/crate/crate-big")
end
