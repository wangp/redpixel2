-- scripts/rpg.lua
-- (experimental) code for the rpg

print("[ rpg.lua ]");

dat = load_datafile("data/rpg.dat")	

rpg = { }
rpg_ammo = { }
rpg_weapon = { }

red.register_weapon_type("rpg", rpg)
red.register_object_type("rpg;ammo", rpg_ammo, red.BITMAP(red.df_item(dat, "ammo")))
--red.register_object_type("rpg;weapon", rpg_weapon, red.dat(dat, )
