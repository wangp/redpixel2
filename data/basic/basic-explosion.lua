-- basic-explosion.lua

store_load ("basic/basic-explosion.dat", "/basic/explosion/")

explosion_type_register (
    "basic-simple42",
    "/basic/explosion/simple42/000", 17, 50/17,
    "/basic/light/white-16",
    "/basic/explosion/simple42/sound"
)
