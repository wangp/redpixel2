-- basic-weapon.lua


store_load ("basic/basic-weapon.dat", "/basic/weapon/")

-- uses nothing
objtype_register ("weapon", "basic-blaster", "/basic/weapon/blaster/pickup")
objtype_register ("projectile",	"basic-blaster-projectile",
                  "/basic/weapon/blaster/projectile")

-- uses arrows			
objtype_register ("weapon", "basic-bow", "/basic/weapon/bow/pickup")
objtype_register ("weapon", "basic-arrow", "/basic/weapon/ammo/arrow")
objtype_register ("projectile", "basic-arrow-projectile",
                  "/basic/weapon/bow/projectile")

-- uses bullets
objtype_register ("weapon", "basic-ak", "/basic/weapon/ak/pickup")
objtype_register ("weapon", "basic-minigun", "/basic/weapon/minigun/pickup")
objtype_register ("weapon", "basic-bullet", "/basic/weapon/ammo/bullet")

-- uses rockets
objtype_register ("weapon", "basic-rpg", "/basic/weapon/rpg/pickup")
objtype_register ("weapon", "basic-rocket", "/basic/weapon/ammo/rocket")
objtype_register ("projectile",	"basic-rocket-projectile", 
                  "/basic/weapon/rpg/projectile")

-- uses shells
objtype_register ("weapon", "basic-shotgun", "/basic/weapon/shotgun/pickup")
objtype_register ("weapon", "basic-shell", "/basic/weapon/ammo/shell")

-- uses sniper bullets
objtype_register ("weapon", "basic-rifle", "/basic/weapon/rifle/pickup")

-- uses mines
objtype_register ("weapon", "basic-mine", "/basic/weapon/mine/pickup")
