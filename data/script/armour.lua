-- armour.lua

local armour = {
}

function __module__init ()
    store_load ("object/armour.dat", "/armour/")
    register_object ("armour-brown",  %armour, "item", "/armour/armour-brown")
    register_object ("armour-purple", %armour, "item", "/armour/armour-purple")
    register_object ("armour-blue",   %armour, "item", "/armour/armour-blue")
end
