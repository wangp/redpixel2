-- scripts/init.lua 
-- load data and initilise scripts, etc.

print("[ init.lua ]");

red.add_tiles_pack("data/tiles.dat")
red.add_tiles_pack("data/tiles2.dat")

dofile("scripts/rpg.lua")
