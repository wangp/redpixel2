-- basic-player.lua

store_load ("basic/basic-player.dat", "/basic/player/")

local basic_player_init = function (self)
    self.mass = 2.2
    self.ramp = 6
    self:add_light ("/basic/light/white-64", 0, 0)
--     self:set_mask (mask_main, "/player/mask-whole", -3, -8)
--     self:set_mask (mask_top, "/player/mask-top", -3, -8)
--     self:set_mask (mask_bottom, "/player/mask-bottom", -3, -8)
--     self:set_mask (mask_left, "/player/mask-left", -3, -8)
--     self:set_mask (mask_right, "/player/mask-right", -3, -8)
end

objtype_register ("player", "player", "/basic/player/walk/000", basic_player_init)
