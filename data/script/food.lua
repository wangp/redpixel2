-- food.lua

function init_burger (self)
    self.mass = 1
end


-- Module init.

store_load ("object/food.dat", "/food/")
objtype_register ("food", "burger",    "/food/burger",	init_burger)
objtype_register ("food", "chocolate", "/food/choc")
objtype_register ("food", "jolt-cola", "/food/jolt")
objtype_register ("food", "medipack",  "/food/medipack")
objtype_register ("food", "pizza",     "/food/pizza")
