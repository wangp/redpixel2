-- basic-light.lua (generates basic-light.dat)

dat = new_datafile ()

function add_lightmap (radius, hue, bright, pinpoint, name)
    local map = create_simple_lightmap (radius, hue, bright, pinpoint)
    local icon = create_lightmap_icon (radius, hue)

    dat = add_to_datafile_magic_bitmap (dat, name, map)
    dat = add_to_datafile_bitmap (dat, name .. "-icon", icon)
end


-- radius, hue (0 - 360), brightness (max 1000), pinpoint (0.0 most focused, 1.0 least focused)

add_lightmap (64,   42, 1000, 0.5, "orange-64")
add_lightmap (32,   42,  800, 0.1, "orange-32")
add_lightmap (16,   42,  800, 0.3, "orange-16")

add_lightmap (64,  108, 1000, 0.5, "lime-64")
add_lightmap (32,  108,  800, 0.1, "lime-32")
add_lightmap (16,  108,  800, 0.3, "lime-16")

add_lightmap (64,  200, 1000, 0.5, "ice-64")
add_lightmap (32,  200,  850, 0.1, "ice-32")
add_lightmap (16,  200,  850, 0.3, "ice-16")

add_lightmap (64,  240, 1000, 0.6, "blue-64")
add_lightmap (32,  240,  850, 0.1, "blue-32")
add_lightmap (16,  240,  850, 0.4, "blue-16")

add_lightmap (64,  273, 1000, 0.2, "violet-64")
add_lightmap (32,  273,  800, 0.1, "violet-32")
add_lightmap (16,  273,  900, 0.1, "violet-16")

add_lightmap (64,   30, 1000, 0.5, "brown-64")
add_lightmap (32,   30,  800, 0.1, "brown-32")
add_lightmap (16,   30,  800, 0.3, "brown-16")

add_lightmap (64,   -1,  600, 0.2, "white-64")
add_lightmap (32,   -1,  600, 0.1, "white-32")
add_lightmap (16,   -1,  600, 0.3, "white-16")

add_lightmap (64,   10, 1000, 0.8, "red-64")
add_lightmap (32,   10,  900, 0.7, "red-32")


save_datafile ("basic-light.dat", dat, 2)
unload_datafile (dat)
