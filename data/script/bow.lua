-- bow.lua

store_load ("object/bow.dat", "/weapon/bow/")
objtype_register ("item", "bow", "/weapon/bow/pickup")
objtype_register ("item", "bow-arrows", "/weapon/bow/ammo")
objtype_register ("projectile", "arrow", "/weapon/bow/projectile")
