-- armour.lua

function __module__init ()
    store_load ("object/armour.dat", "/armour/")
    object_type_register ("armour-brown",  nil, "item", "/armour/armour-brown")
    object_type_register ("armour-purple", nil, "item", "/armour/armour-purple")
    object_type_register ("armour-blue",   nil, "item", "/armour/armour-blue")
end
