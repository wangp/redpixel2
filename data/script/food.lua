-- food.lua

local food = {
}

function __module__init ()
    store_load ("object/food.dat", "/food/")

    register_object ("burger", %food, "food", "/food/burger")
    register_object ("chocolate", %food, "food", "/food/choc")
    register_object ("jolt-cola", %food, "food", "/food/jolt")
    register_object ("medipack", %food, "food", "/food/medipak")
    register_object ("pizza", %food, "food",  "/food/pizza")
end
