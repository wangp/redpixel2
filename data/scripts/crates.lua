-- crates.lua

local crate = {
}

function __module__init ()
    store_load ("objects/crates.dat", "/crates/")
    register_object (%crate, "objtiles", "/crates/crate1")
    register_object (%crate, "objtiles", "/crates/crate2")
    register_object (%crate, "objtiles", "/crates/cratebig")
end
