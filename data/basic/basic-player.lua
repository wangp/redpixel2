-- basic-player.lua

store_load ("basic/basic-player.dat", "/basic/player/")


----------------------------------------------------------------------
--  Players
----------------------------------------------------------------------


local xv_decay, yv_decay = 0.75, 0.5

-- centre of the player sprites
local cx, cy = 5, 5


--
-- Non-proxy
--

local player_nonproxy_init = function (self)
    -- some properties
    self.xv_decay = xv_decay
    self.yv_decay = yv_decay
    self.mass = 0.9
    self._internal_ramp = 6

    -- collision stuff
    object_set_collision_is_player (self)
    self:set_collision_flags ("tn")
    self:set_mask (mask_main, "/basic/player/mask/whole", cx, cy)
    self:set_mask (mask_top, "/basic/player/mask/top", cx, cy)
    self:set_mask (mask_bottom, "/basic/player/mask/bottom", cx, cy)
    self:set_mask (mask_left, "/basic/player/mask/left", cx, cy)
    self:set_mask (mask_right, "/basic/player/mask/right", cx, cy)

    -- weapon stuff
    self.current_weapon = weapons["basic-blaster"] -- XXX temp
    function self:receive_weapon (weapon_name)
	if not weapons[weapon_name] then
	    self.have_weapon[weapon_name] = 1
	    if not self.current_weapon then
		self.current_weapon = weapon[weapon_name]
	    end
	end
    end

    -- firing stuff
    self.fire_delay = 0
    function self:_internal_fire_hook ()
	if self.fire_delay <= 0 then
	    local w = self.current_weapon
	    if w and w.can_fire (self) then
		w.fire (self)
	    end
	end
    end

    -- update hook
    self:set_update_hook (
	1000/50,
	function (self)
	    if self.fire_delay > 0 then
		self.fire_delay = self.fire_delay - 1
	    end
	end
    )

    -- health stuff
    self.health = 100
    function self:receive_damage (damage)
	spawn_blood (self.x + cx, self.y + cy, 100, 2)
	spawn_blod (self.x, self.y, 10)
	self.health = self.health - damage
	if self.health <= 0 then
	    spawn_object ("basic-player-death-fountain", self.x, self.y)
	    self:destroy ()
	end
    end
end


--
-- Proxy
--

local animate_player_proxy = function (self)
    if not _internal_object_moving_horizontally (self) then
	return
    end
    
    self.walk_frame_tics = self.walk_frame_tics - 1
    if self.walk_frame_tics <= 0 then
	self.walk_frame_tics = 2

	self.walk_frame = self.walk_frame + 1
	if self.walk_frame > 7 then
	    self.walk_frame = 0
	end
	self:replace_layer (0, format ("/basic/player/walk/%03d", self.walk_frame), cx, cy)
    end
end

local rotate_and_flip_player_proxy_based_on_aim_angle = function (self)
    if self.last_aim_angle == self.aim_angle then
	return
    end

    self.last_aim_angle = self.aim_angle

    local angle = radian_to_bangle (self.aim_angle)
    local hflip = (angle < -63 or angle > 63)
    self:hflip_layer (0, hflip and 1 or 0)
    self:hflip_layer (self.arm_layer, hflip and 1 or 0)
    self:rotate_layer (self.arm_layer, angle - (hflip and 128 or 0))
end

local player_proxy_init = function (self)
    -- some properties
    self.xv_decay = xv_decay
    self.yv_decay = yv_decay

    -- layers
    self:move_layer (0, cx, cy)
    self.arm_layer = self:add_layer ("/basic/weapon/blaster/1arm000", 0, 3)

    -- light
    self:add_light (self.is_local and "/basic/light/white-64" or "/basic/light/white-32", 0, 0)

    -- animation and update hook
    self.walk_frame = 0
    self.walk_frame_tics = 0
    self:set_update_hook (
	1000/50,
	function (self)
	    animate_player_proxy (self)
	    rotate_and_flip_player_proxy_based_on_aim_angle (self)
	end
    )
end


Objtype {
    category = "player",    -- XXX: should be nil in future; the map editor should not allow it to be placed
    name = "player",	    -- not basic-player
    icon = "/basic/player/walk/000",
    nonproxy_init = player_nonproxy_init,
    proxy_init = player_proxy_init
}


----------------------------------------------------------------------
--  Corpses
----------------------------------------------------------------------

local death_fountain_update_hook = function (self)
    self:replace_layer (0,format ("/basic/player/death-fountain/%03d", self.frame), 44, 16)
    self.frame = self.frame + 1
    if self.frame > 30 then
	self.frame = 30
	if not self.is_proxy then
	    self:remove_update_hook ()
	else
	    -- proxy: let the light remain for half a second more
	    self:set_update_hook (
		1000/2,
		function (self)
		    self:remove_all_lights ()
		    self:remove_update_hook ()
		end
	    )
	end
    end
end

Objtype {
    category = "player",
    name = "basic-player-death-fountain",
    icon = "/basic/player/death-fountain/000",

    nonproxy_init = function (self)
	self:set_masks_centre (44, 16)
	self:set_collision_flags ("t")
	self.mass = 0.005
	self.frame = 0
	self:add_creation_field ("frame")
	self:set_update_hook (1000/10, death_fountain_update_hook)
    end,

    proxy_init = function (self)
	self:add_light ("/basic/light/white-32", 0, 0)
	self:set_update_hook (1000/10, death_fountain_update_hook)
	death_fountain_update_hook (self) -- set the initial layer
    end
}
