-- scripts/rpg.lua
-- (experimental) code for the rpg

rpg_data = { dat, bammo, bweapon, bstatus, bproj }

ammo = {
}


weapon = {
}

rpg = {

	new = function(self,it)
		-- do stuff here
	end,

	draw_status = function(self,dest,x,y)
		draw_sprite(dest,rpg_data.bstatus,x,y)
	end
}



dat = load_datafile("data/rpg.dat")	
--if not dat then return -1 end

rpg_data.bammo   = df_dat(df_get_item(dat, "ammo"))
rpg_data.bproj   = df_dat(df_get_item(dat, "projectile"))
rpg_data.bstatus = df_dat(df_get_item(dat, "status"))
rpg_data.bweapon = df_dat(df_get_item(dat, "weapon"))

register_object_type("rpg;ammo", ammo)
register_object_type("rpg;weapon", weapon)
register_object_type("rpg;rpg", rpg)
