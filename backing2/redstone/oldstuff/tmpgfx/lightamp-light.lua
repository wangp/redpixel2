dat = new_datafile ()

-- radius, hue (0 - 360), brightness (max 1000), pinpoint (0.0 most focused, 1.0 least focused)

light = create_simple_lightmap (128, 220, 1000, 0.5)

dat = add_to_datafile_magic_bitmap (dat, "lightamp/light", light)

save_datafile ("basic-lightamplight.dat", dat, 2)
unload_datafile (dat)
