-- basic-player.lua

store_load ("basic/basic-player.dat", "/basic/player/")


local corpses = {}


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

    function self:receive_weapon (name)
	if weapons[name] and not self.have_weapon[name] then
	    self.have_weapon[name] = true
	    if contains (weapon_auto_switch_order, name) then
		self:switch_weapon (name)
	    end
	end
    end

    function self:switch_weapon (name)
	if weapons[name] and self.have_weapon[name] then
	    self.current_weapon = weapons[name]
	    self.desired_weapon = weapons[name]
	    call_method_on_clients (self, "switch_weapon", name)
	end
    end

    function self:auto_switch_weapon ()
	for _, name in weapon_auto_switch_order do
	    if (weapons[name] and self.have_weapon[name] and
		weapons[name].can_fire (self))
	    then
		self.current_weapon = weapons[name]
		call_method_on_clients (self, "switch_weapon", name)
		break
	    end
	end
    end

    self:receive_weapon ("basic-blaster")

    -- ammo stuff
    self._ammo = {}

    function self:receive_ammo (ammo_type, amount)
	self._ammo[ammo_type] = (self._ammo[ammo_type] or 0) + amount

	if self.desired_weapon ~= self.current_weapon and
	    self.desired_weapon.can_fire (self) and
	    contains (weapon_auto_switch_order, self.desired_weapon.name)
	then
	    self.current_weapon = self.desired_weapon
	    call_method_on_clients (self, "switch_weapon", self.current_weapon.name)
	end
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
	    if w and w.can_fire (self) then
		w.fire (self)
		call_method_on_clients (self, "start_firing_anim")
	    else
		self:auto_switch_weapon ()
	    end
	end
    end

    -- health stuff
    self.health = 100

    function self:receive_damage (damage)
	spawn_blood (self.x + cx, self.y + cy, 100, 2)
	if damage/5 >= 1 then
	    spawn_blod (self.x, self.y, damage/5)
	end
	self.health = self.health - damage
	if self.health <= 0 then
	    local corpse = spawn_object (corpses[random (getn (corpses))],
					 self.x, self.y)
	    if corpse then
		-- this makes the client track the corpse
		corpse._internal_stalk_me = self.id
		corpse:add_creation_field ("_internal_stalk_me")

		-- drop a backpack after the corpse finishes animation
		local x, y = self.x, self.y
		corpse.drop_backpack = function ()
		    spawn_object ("basic-backpack", x, y)
		end
	    end
	    self:destroy ()
	else
	    call_method_on_clients (self, "start_alt_light",
		"return '/basic/light/red-64', 38, 10")
	end
    end

    function self:receive_health (amount)
	self.health = min (100, self.health + amount)
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

    -- create a respawning ball where the player is
    spawn_object ("basic-respawning-ball", self.x, self.y)
end


--
-- Proxy
--

local walk_anim = {}
for i = 0,7 do
    walk_anim[i] = format ("/basic/player/walk/%03d", i)
end

local animate_player_proxy_firing = function (self)
    if self.animate_arm then
	if self.arm_tics > 0 then
	    self.arm_tics = self.arm_tics - 1
	elseif self.current_weapon then
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
end

local animate_player_proxy_walking = function (self)
    if not _internal_object_moving_horizontally (self) then
	return
    end
    
    if self.walk_frame_tics > 0 then
	self.walk_frame_tics = self.walk_frame_tics - 1
    else
	self.walk_frame_tics = 2

	self.walk_frame = self.walk_frame + 1
	if self.walk_frame >= getn (walk_anim) then
	    self.walk_frame = 0
	end
	self:replace_layer (0, walk_anim[self.walk_frame], cx, cy)
    end
end

local animate_player_proxy_alt_lighting = function (self)
    if self.alt_light_tics > 0 then
	self.alt_light_tics = self.alt_light_tics - 1
	if self.alt_light_tics == 0 then
	    self:replace_light (0, "/basic/light/white-64", 0, 0)
	    self.alt_light_priority = 0
	end
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

    self.alt_light_tics = 0
    self.alt_light_priority = 0

    function self:start_alt_light (this_is_dodgy)
	local the_light, timeout, priority = dostring (this_is_dodgy) -- XXX not good for security
	if (priority > self.alt_light_priority) and self.is_local then
	    self:replace_light (0, the_light, 0, 0)
	    self.alt_light_tics = timeout
	    self.alt_light_priority = priority
	end
    end

    -- arm stuff
    self.arm_layer = self:add_layer ("/basic/weapon/blaster/1arm000", 0, 3)
    self.arm_frame = 1

    -- (called by nonproxy fire hook)
    function self:start_firing_anim ()
	if not self.animate_arm and self.current_weapon then
	    self.animate_arm = true
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
	    animate_player_proxy_firing (self)
	    animate_player_proxy_walking (self)
	    animate_player_proxy_alt_lighting (self)
	    rotate_and_flip_player_proxy_based_on_aim_angle (self)
	end
    )

    -- switch weapon (called by nonproxy switch_weapon)
    function self:switch_weapon (weapon_name)
	local w = weapons[weapon_name]
	self.current_weapon = w
	self:replace_layer (self.arm_layer, w.arm_anim[1], w.arm_anim.cx, w.arm_anim.cy)
	if self.is_local then
	    _internal_set_camera (0, 96)
	    if w.client_switch_to_hook then
		w.client_switch_to_hook ()
	    end
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

local Corpse = function (t)

    local anim = {}
    for i = 0, t.frames do
	anim[i] = format (t.fmt, i)
    end

    -- Note: both proxy and nonproxy have to update self.frames.

    local nonproxy_update = function (self)
	self.frame = self.frame + 1
	if self.frame > t.frames then
	    self.frame = t.frames
	    if self.drop_backpack then
		-- drop the backpack after a little whil
		self:set_update_hook (
		    1000/3,
		    function (self)
			self.drop_backpack ()
			self:remove_update_hook ()
		    end
		)
	    else
		self:remove_update_hook ()
	    end
	end
    end

    local proxy_update = function (self)
	self:replace_layer (0, anim[self.frame], t.cx, t.cy)
	self.frame = self.frame + 1
	if self.frame > t.frames then
	    self.frame = t.frames
	    -- let the light remain for half a second more
	    self:set_update_hook (
		1000/2,
		function (self)
		    self:remove_all_lights ()
		    self:remove_update_hook ()
		end
	    )
	end
    end

    if not t.speed then
	t.speed = 1000/8
    end

    tinsert (corpses, t.name)

    return Objtype (t, {
    
	icon = anim[0],
	
	nonproxy_init = function (self)
	    if t.mask then
		self:set_mask (mask_main, t.mask, t.cx, t.cy)
		self:set_mask (mask_bottom, t.mask, t.cx, t.cy)
	    end
	    self:set_masks_centre (t.cx, t.cy)
	    self:set_collision_flags ("t")
	    self.mass = 0.005
	    self.frame = 0
	    self:add_creation_field ("frame")
	    self:set_update_hook (t.speed, nonproxy_update)
	end,
	
	proxy_init = function (self)
	    -- self.frame is given by server
	    self:add_light ("/basic/light/red-32", 0, 0)
	    self:set_update_hook (t.speed, proxy_update)
	    proxy_update (self) -- set the initial layer
	end
    })
end

Corpse {
    name = "basic-player-death-arm",
    alias = "~da",
    fmt = "/basic/player/death-arm/%03d",
    mask = "/basic/player/death-arm/mask",
    frames = 16,
    cx = 26,
    cy = 8
}

Corpse {
    name = "basic-player-death-eye",
    alias = "~de",
    fmt = "/basic/player/death-eye/%03d",
    mask = "/basic/player/death-eye/mask",
    frames = 20,
    cx = 26,
    cy = 9
}

Corpse {
    name = "basic-player-death-fountain",
    alias = "~df",
    fmt = "/basic/player/death-fountain/%03d",
    mask = "/basic/player/death-fountain/mask",
    frames = 30,
    speed = 1000/10,
    cx = 44,
    cy = 16
}

Corpse {
    name = "basic-player-death-leg",
    alias = "~dl",
    fmt = "/basic/player/death-leg/%03d",
    mask = "/basic/player/death-leg/mask",
    frames = 15,
    speed = 1000/7,
    cx = 26,
    cy = 8
}

Corpse {
    name = "basic-player-death-leg2",
    alias = "~dL",
    fmt = "/basic/player/death-leg2/%03d",
    mask = "/basic/player/death-leg2/mask",
    frames = 17,
    speed = 1000/6,
    cx = 21,
    cy = 8
}

Corpse {
    name = "basic-player-death-stomach",
    alias = "~ds",
    fmt = "/basic/player/death-stomach/%03d",
    mask = "/basic/player/death-stomach/mask",
    frames = 16,
    cx = 13,
    cy = 8
}


----------------------------------------------------------------------
--  Backpack
----------------------------------------------------------------------

Objtype {
    name = "basic-backpack",
    icon = "/basic/player/backpack",
    nonproxy_init = function (self)
	self.mass = 0.01
	self:set_collision_flags ("pt")
	function self:collide_hook (player)
	    -- give goodies
	    self:destroy ()
	end
    end
}
