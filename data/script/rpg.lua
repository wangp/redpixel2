-- rpg.lua

local rpg = {
}

function __module__init ()
    store_load ("object/rpg.dat", "/weapon/rpg/")
    object_type_register ("rpg",         %rpg, "item", "/weapon/rpg/pickup")
    object_type_register ("rpg-rockets",  nil, "item", "/weapon/rpg/ammo")
end
