-- food.lua

function __module__init ()
    store_load ("object/food.dat", "/food/")

    object_type_register ("burger",    nil, "food", "/food/burger")
    object_type_register ("chocolate", nil, "food", "/food/choc")
    object_type_register ("jolt-cola", nil, "food", "/food/jolt")
    object_type_register ("medipack",  nil, "food", "/food/medipak")
    object_type_register ("pizza",     nil, "food", "/food/pizza")
end
