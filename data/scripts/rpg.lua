-- rpg.lua

function __module__init ()
    store_load ("objects/rpg.dat", "/weapons/rpg/")
    register_object (nil, "item", "/weapons/rpg/pickup")
    register_object (nil, "item", "/weapons/rpg/ammo")
end
