-- basic-player.lua

store_load ("basic/basic-player.dat", "/basic/player/")


----------------------------------------------------------------------
--  Players
----------------------------------------------------------------------


local xv_decay, yv_decay = 0.7, 0.45

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
    self.have_weapon = {}

    function self:receive_weapon (weapon_name)
	if weapons[weapon_name] then
	    self.have_weapon[weapon_name] = 1
	    self:switch_weapon (weapon_name)
	end
    end

    function self:switch_weapon (weapon_name)
	if weapons[weapon_name] and self.have_weapon[weapon_name] then
	    self.current_weapon = weapons[weapon_name]
	    call_method_on_clients (self, "switch_weapon", weapon_name)
	end
    end

    self:receive_weapon ("basic-blaster")

    -- ammo stuff
    self._ammo = {}

    function self:receive_ammo (ammo_type, amount)
	self._ammo[ammo_type] = (self._ammo[ammo_type] or 0) + amount
    end

    function self:deduct_ammo (ammo_type, amount)
	self._ammo[ammo_type] = max (0, (self._ammo[ammo_type] or 0) - (amount or 1))
    end

    function self:ammo (ammo_type)
	return self._ammo[ammo_type] or 0
    end

    -- firing stuff
    self.fire_delay = 0
    function self:_internal_fire_hook ()
	if self.fire_delay <= 0 then
	    local w = self.current_weapon
	    if w and (not w.can_fire or w.can_fire (self)) then
		w.fire (self)
		call_method_on_clients (self, "start_firing_anim")
	    end
	end
    end

    -- health stuff
    self.health = 100
    function self:receive_damage (damage)
	spawn_blood (self.x + cx, self.y + cy, 100, 2)
	spawn_blod (self.x, self.y, 10)
	self.health = self.health - damage
	if self.health <= 0 then
	    local corpse = spawn_object ("basic-player-death-fountain",
					 self.x, self.y)
	    if corpse then
		-- this makes the client track the corpse
		corpse._internal_stalk_me = self.id
		corpse:add_creation_field ("_internal_stalk_me")
	    end
	    self:destroy ()
	end
    end

    -- update hook (fire delay and blood trails)
    self.trail_tics = 0
    self:set_update_hook (
	1000/50,
	function (self)
	    if self.fire_delay > 0 then
		self.fire_delay = self.fire_delay - 1
	    end

	    if self.health <= 20 then
		if self.trail_tics > 0 then
		    self.trail_tics = self.trail_tics - 1
		else
		    self.trail_tics = 50
		    spawn_blood (self.x + cx, self.y + cx, 40, 2)
		end
	    end
	end
    )
end


--
-- Proxy
--

local animate_player_proxy = function (self)
    -- firing
    if self.animate_arm then
	if self.arm_tics > 0 then
	    self.arm_tics = self.arm_tics - 1
	else
	    local anim = self.current_weapon.arm_anim
	    self.arm_tics = anim.tics or 5
	    self.arm_frame = self.arm_frame + 1
	    if self.arm_frame > self.last_arm_frame then
		self.arm_frame = 1
		self.animate_arm = false
	    end
	    self:replace_layer (self.arm_layer, anim[self.arm_frame], anim.cx, anim.cy)
	end
    end

    -- walking
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

    -- light
    self:add_light (self.is_local and "/basic/light/white-64" or "/basic/light/white-32", 0, 0)

    -- arm stuff
    self.arm_layer = self:add_layer ("/basic/weapon/blaster/1arm000", 0, 3)
    self.arm_frame = 1

    -- (called by nonproxy fire hook)
    function self:start_firing_anim ()
	if not self.animate_arm then
	    self.animate_arm = 1
	    self.arm_tics = 0
	    self.last_arm_frame = getn (self.current_weapon.arm_anim)
	end
    end

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

    -- switch weapon (called by nonproxy switch_weapon)
    function self:switch_weapon (weapon_name)
	local w = weapons[weapon_name]
	self.current_weapon = w
	self:replace_layer (self.arm_layer, w.arm_anim[1],
			    w.arm_anim.cx, w.arm_anim.cy)
	_internal_set_camera (0, 96)
	if self.is_local and w.client_switch_to_hook then
	    w.client_switch_to_hook ()
	end
    end
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
