-- lightmap.lua (generates lightmap.dat)

dat = new_datafile ()

function add_lightmap (radius, hue, bright, pinpoint, name)
    local map = create_simple_lightmap (radius, hue, bright, pinpoint)
    local icon = create_lightmap_icon (radius, hue)

    dat = add_to_datafile_magic_bitmap (dat, name, map)
    dat = add_to_datafile_bitmap (dat, name .. "-icon", icon)
end


-- radius, hue (0 - 360), brightness (max 1000), pinpoint (0.0 most focused, 1.0 least focused)

add_lightmap (64,   42, 1000, 0.5, "orange-small")
add_lightmap (32,   42,  800, 0.1, "orange-tiny")

add_lightmap (64,  108, 1000, 0.5, "lime-small")
add_lightmap (32,  108,  800, 0.1, "lime-tiny")

add_lightmap (64,  200, 1000, 0.5, "ice-small")
add_lightmap (32,  200,  850, 0.1, "ice-tiny")

add_lightmap (64,  240, 1000, 0.6, "blue-small")
add_lightmap (32,  240,  850, 0.1, "blue-tiny")

add_lightmap (64,  273, 1000, 0.2, "violet-small")
add_lightmap (32,  273,  800, 0.1, "violet-tiny")

add_lightmap (64,   30, 1000, 0.5, "brown-small")
add_lightmap (32,   30,  800, 0.1, "brown-tiny")

add_lightmap (64,   -1,  600, 0.2, "white-small")
add_lightmap (32,   -1,  600, 0.1, "white-tiny")


save_datafile ("lightmap.dat", dat, 1)
unload_datafile (dat)


-- end of lightmap.lua
