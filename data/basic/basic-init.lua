-- basic-init.lua

dofile ("basic/basic-base.lua")

tiles_load ("basic/basic-tile.dat", "/basic/tile/")
lights_load ("basic/basic-light.dat", "/basic/light/")
store_load ("basic/basic-crosshair.dat", "/basic/crosshair/")
dofile ("basic/basic-tilelike.lua")
dofile ("basic/basic-powerup.lua")
dofile ("basic/basic-weapon.lua")
dofile ("basic/basic-player.lua")
