dat = new_datafile ()

-- radius, hue (0 - 360), brightness (max 1000), pinpoint (0.0 most focused, 1.0 least focused)
function add_lightmap (radius, hue, bright, pinpoint, name)
    local map = create_simple_lightmap (radius, hue, bright, pinpoint)
    dat = add_to_datafile_magic_bitmap (dat, name, map)
end

-- want cyan
add_lightmap (8,   300,  600, 0.7, "light")

save_datafile ("basic-respawnlight.dat", dat, 2)
unload_datafile (dat)
