-- basepack.lua (generates basepack.dat)

function make_tile_pack (datname, images) 
    local dat = new_datafile ()

    local name, filename = next (images, nil)
    while name do
	dat = add_to_datafile_grab_from_grid (dat, name, filename,
					      16, -- depth
					      16, -- gridx
					      16) -- gridy
	name, filename = next (images, name)
    end
    
    save_datafile (datname, dat,
		   0,		-- strip
		   1,		-- compression
		   1,		-- verbose
    		   1,		-- write_msg
		   0)		-- backup

    unload_datafile (dat)
end


images = {}
images ["bricks"]      = "bricks.png"
images ["bricks/dark"] = "bricks-dark.png"
images ["dirt"]        = "dirt-dark.png"
images ["grass"]       = "grass2-dark.png"
images ["marble"]      = "marble.png"
images ["steel"]       = "steel-en.png"

make_tile_pack ("basepack.dat", images)


-- end of basepack.lua
