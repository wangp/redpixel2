-- tilehelp.lua

function make_tile_pack (datname, prefix, images) 
    local dat = new_datafile ()

    local name, filename = next (images, nil)
    while name do
	dat = add_to_datafile_grab_from_grid (dat, name, 
					      prefix .. filename,
					      16, -- depth
					      16, -- gridx
					      16) -- gridy
	name, filename = next (images, name)
    end
    
    save_datafile (datname, dat, 2)
    unload_datafile (dat)
end

-- end of tilehelp.lua
