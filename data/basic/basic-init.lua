-- basic-init.lua

dofile ("basic/basic-base.lua")

tiles_load ("basic/basic-tile.dat", "/basic/tile/")
lights_load ("basic/basic-light.dat", "/basic/light/")
store_load ("basic/basic-crosshair.dat", "/basic/crosshair/")
store_load ("basic/basic-font.dat", "/basic/font/")
dofile ("basic/basic-tilelike.lua")
dofile ("basic/basic-powerup.lua")
dofile ("basic/basic-weapon.lua")
dofile ("basic/basic-player.lua")
dofile ("basic/basic-explosion.lua")
dofile ("basic/basic-gametype.lua")
