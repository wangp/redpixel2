-- basic-explosion.lua

store_load ("basic/basic-explosion.dat", "/basic/explosion/")

explosion_type_register (
    "basic-explo20",
    "/basic/explosion/explo20/000", 16, ticks_per_second/20,
    "/basic/light/white-16",
    "/basic/explosion/explo42/sound" --XXX
)

explosion_type_register (
    "basic-explo42",
    "/basic/explosion/explo42/000", 17, ticks_per_second/17,
    "/basic/light/white-16",
    "/basic/explosion/explo42/sound"
)
