-- scripts/init.lua 
-- load data and initilise scripts, etc.

print("Executing init.lua")

local tiles = { "tiles.dat", "tiles2.dat" }
local weapons = { "rpg.sc", "shotgun.sc" }
local i,v,f

-- tiles

i,v = next(tiles, nil)
while i do
	f = "data/"..v
  	--add_tiles_pack(f)
  	i,v = next(tiles, i)
end

-- weapons

i,v = next(weapons, nil)
while i do
  	f = "scripts/"..v
  	dofile(f)
  	i,v = next(tiles, i)
end
