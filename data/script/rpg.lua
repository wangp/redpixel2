-- rpg.lua

function __module__init ()
    store_load ("object/rpg.dat", "/weapon/rpg/")
    register_object ("rpg",         nil, "item", "/weapon/rpg/pickup")
    register_object ("rpg-rockets", nil, "item", "/weapon/rpg/ammo")
end
