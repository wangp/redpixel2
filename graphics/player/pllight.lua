-- pllight.lua (generates pllight.dat)

dat = new_datafile ()

map = create_lightmap_from_bitmap ("torch-big.png")
dat = add_to_datafile_magic_bitmap (dat, "torch", map)

save_datafile ("pllight.dat", dat, 2)
unload_datafile (dat)

-- end of pllight.lua
