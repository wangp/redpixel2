-- food.lua

local food = {
    player_touch = function (self, player)

-- 	if self.icon == "/food/burger" then
-- 	    player:give_health (10)
-- 	end

    end
}

function __module__init ()
    store_load ("objects/food.dat", "/food/")
    register_object (nil, "food", "/food/burger")
    register_object (nil, "food", "/food/choc")
    register_object (nil, "food", "/food/jolt")
    register_object (nil, "food", "/food/medipak")
    register_object (nil, "food", "/food/pizza")
end
