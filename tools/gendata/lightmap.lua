-- lightmap.lua (generates lightmap.dat)

dat = new_datafile ()

function add_lightmap (radius, hue, bright, pinpoint, name)
    local map = create_simple_lightmap (radius, hue, bright, pinpoint)
    local icon = create_lightmap_icon (radius, hue)

    dat = add_to_datafile_bitmap (dat, name, map)
    dat = add_to_datafile_bitmap (dat, name .. "-icon", icon)
end

function add_mono_lightmap (radius, bright, pinpoint, name)
    local map = create_mono_lightmap (radius, bright, pinpoint)
    local icon = create_lightmap_icon (radius, hue)

    dat = add_to_datafile_bitmap (dat, name, map)
    dat = add_to_datafile_bitmap (dat, name .. "-icon", icon)
end


-- radius, hue (0 - 360), brightness (max 1000), pinpoint (0.0 most focused, 1.0 least focused)

add_lightmap (128,  42, 400, 0.5, "orange-huge")
add_lightmap (64,   42, 400, 0.5, "orange-small")

add_lightmap (128, 108, 500, 0.5, "lime-huge")
add_lightmap (64,  108, 500, 0.5, "lime-small")

add_lightmap (128, 200, 500, 0.5, "ice-huge")
add_lightmap (64,  200, 500, 0.5, "ice-small")

add_lightmap (128, 240, 700, 0.5, "blue-huge")
add_lightmap (64,  240, 700, 0.5, "blue-small")

add_lightmap (128, 273, 450, 0.5, "violet-huge")
add_lightmap (64,  273, 450, 0.5, "violet-small")

-- XXX
add_mono_lightmap (128, 500, 0.2, "white-huge")
add_mono_lightmap (64,  500, 0.2, "white-small")

save_datafile ("lightmap.dat", dat, 0, 
		1,		-- compression
		0, 1, 0)
unload_datafile (dat)