-- basic-player.lua
--	Player, corpse, backpack stuff.
--	Some weapon and powerup specific stuff too.


store_load ("basic/basic-player.dat", "/basic/player/")


-- holds the various types of corpses
local corpses = {}



----------------------------------------------------------------------
--  Helpers for complicated update hooks
--  (move out of here if other files need it too one day)
----------------------------------------------------------------------

function install_multiple_update_hook_system (obj, speed)
    obj._update_hooks = {}

    -- Add a new hook PROC to be called every NTICS.
    -- These "tics" are NOT the same as game time ticks, but are
    -- related to the update hook's speed.
    -- Returns a new hook structure (don't touch it!)
    function obj:add_update_hook (ntics, enabled, proc)
	local t = {
	    tics = ntics,
	    ntics = ntics,
	    enabled = enabled,
	    proc = proc
	}
	table.insert (self._update_hooks, t)
	return t
    end

    -- Make a registered hook be called every NTICS.  The current
    -- counter is reset.
    function obj:adjust_update_hook_speed (t, ntics)
	t.ntics = ntics
	t.tics = ntics
    end

    -- Enable a previously disabled hook.
    function obj:enable_update_hook (t)
	t.enabled = true
    end

    -- This is the raw update hook that does the work of dispatching
    -- registered hooks.  When a registered hook is called, it may
    -- return `false' to disable itself.
    obj:set_update_hook (
	speed,
	function (self)
	    for i, t in self._update_hooks do
		if i ~= "n" and t.enabled then
		    t.tics = t.tics - 1
		    if t.tics < 1 then
			t.tics = t.ntics
			if t.proc (self) == false then
			    t.enabled = false
			end
		    end
		end
	    end
	end
    )

    -- After this, the set_update_hook method no longer makes sense.
    obj.set_update_hook = nil
end



----------------------------------------------------------------------
--  Players
----------------------------------------------------------------------


-- xv_decay = 0.7 -- normal
-- xv_decay = 0.685 -- medium
-- xv_decay = 0.68 -- snail pace
-- needs to be send to clients as well

-- yv_decay = 0.45 -- demo2 speed
-- yv_decay = 0.41 -- probably better, but floats down stairs too much..

--local xv_decay, yv_decay = 0.7, 0.41
local xv_decay, yv_decay = 0.67, 0.41

-- centre of the player sprites (really?)
local cx, cy = 5, 5

local player_update_hook_speed = msecs_per_tick

function secs_to_ticks (secs)
    return (secs * 1000) / player_update_hook_speed
end



------------------------------------------------------------
--  Non-proxy player object
------------------------------------------------------------


local player_nonproxy_init = function (self)

    install_multiple_update_hook_system (self, player_update_hook_speed)


    --------------------------------------------------
    --  Some properties
    --------------------------------------------------

    self.xv_decay = xv_decay
    self.yv_decay = yv_decay
    self.mass = 0.9
    self._internal_ramp = 6
    self.is_player = true


    --------------------------------------------------
    --  Collision stuff
    --------------------------------------------------

    object_set_collision_is_player (self)
    self:set_collision_flags ("tn")
    self:set_mask (mask_main, "/basic/player/mask/whole", cx, cy)
    self:set_mask (mask_top, "/basic/player/mask/top", cx, cy)
    self:set_mask (mask_bottom, "/basic/player/mask/bottom", cx, cy)
    self:set_mask (mask_left, "/basic/player/mask/left", cx, cy)
    self:set_mask (mask_right, "/basic/player/mask/right", cx, cy)


    --------------------------------------------------
    --  Weapon stuff
    --------------------------------------------------

    self.have_weapon = {}

    function self:receive_weapon (name)
	if weapons[name] and not self.have_weapon[name] then

	    self.have_weapon[name] = true

	    -- tell the proxy object about the new weapon 
	    call_method_on_clients (self, "get_new_weapon_hook", name)

	    -- if this is the first weapon, always switch to it
	    -- (it will be the blaster anyway)
	    if not self.current_weapon then
		self:switch_weapon (name)
		return
	    end


	    -- check the new weapon is auto-switchable
	    if not contains (weapon_auto_switch_order, name) then
		return
	    end

	    -- don't switch if our current weapon is not auto-switchable
	    if not contains (weapon_auto_switch_order, self.current_weapon.name) then
		return
	    end

	    -- if our current weapon is out of ammo, switch
	    -- if the new weapon is better than the current weapon, switch
	    if not self:has_ammo_for (self.current_weapon) or
	       (index_of (weapon_auto_switch_order, name) < 
		index_of (weapon_auto_switch_order, self.current_weapon.name))
	    then
		self:switch_weapon (name)
	    end
	end
    end

    function self:post_weapon_switch_hook ()
	local w = self.current_weapon
	call_method_on_clients (self, "switch_weapon_hook", w.name)
	_internal_tell_ammo (self, self._ammo[w.ammo_type] or 0)
    end

    function self:switch_weapon (name)
	-- handle up/down switching instead of direct switching
	if name == "_internal_next" then
	    self:switch_weapon_up_down_helper (1)
	elseif name == "_internal_prev" then
	    self:switch_weapon_up_down_helper (-1)

	-- otherwise name is really the name of a weapon
	elseif weapons[name] and self.have_weapon[name] then
	    self.current_weapon = weapons[name]
	    self.desired_weapon = weapons[name]
	    self:post_weapon_switch_hook ()
	end
    end

    function self:switch_weapon_up_down_helper (direction)
	local i = index_of (weapon_order, self.current_weapon.name)
	if i then
	    while true do
		i = i + direction
		local name = weapon_order[i]
		if name == nil then
		    break
		end
		if self.have_weapon[name] and self:has_ammo_for (name) then
		    self:switch_weapon (name)
		    break
		end
	    end
	end
    end

    function self:auto_switch_weapon ()
	for _, name in weapon_auto_switch_order do
	    if (weapons[name] and self.have_weapon[name] and
		self:has_ammo_for (name))
	    then
		self.current_weapon = weapons[name]
		self:post_weapon_switch_hook ()
		break
	    end
	end
    end


    --------------------------------------------------
    --  Ammo stuff
    --------------------------------------------------

    self._ammo = {}

    function self:receive_ammo (ammo_type, amount)
	self._ammo[ammo_type] = (self._ammo[ammo_type] or 0) + amount

	if self.desired_weapon ~= self.current_weapon and
	    self:has_ammo_for (self.desired_weapon) and
	    contains (weapon_auto_switch_order, self.desired_weapon.name)
	then
	    self.current_weapon = self.desired_weapon
	    self:post_weapon_switch_hook ()
	elseif ammo_type == self.current_weapon.ammo_type then
	    _internal_tell_ammo (self, self._ammo[ammo_type])
	end
    end

    function self:deduct_ammo (ammo_type, amount)
	local v = math.max (0, (self._ammo[ammo_type] or 0) - (amount or 1))
	self._ammo[ammo_type] = v
	_internal_tell_ammo (self, v)
    end

    function self:has_ammo_for (weapon)
	local ammo_type
	if type (weapon) == "string" then
	    ammo_type = weapons[weapon].ammo_type
	else
	    ammo_type = weapon.ammo_type
	end
	if ammo_type == nil then
	    return true
	end
	local ammo = self._ammo[ammo_type]
	return ammo ~= nil and ammo > 0
    end


    --------------------------------------------------
    --  Firing stuff
    --------------------------------------------------

    -- fire delay stuff

    self.can_fire = true

    local reenable_firing_hook = self:add_update_hook (
	0,			-- dummy speed
	false,			-- initially disabled
	function (self)
	    self.can_fire = true
	    return false
	end
    )

    function self:set_fire_delay (secs)
	self.can_fire = false
	self:adjust_update_hook_speed (reenable_firing_hook, secs_to_ticks (secs))
	self:enable_update_hook (reenable_firing_hook)
    end

    -- damage factor stuff

    self.damage_factor = 1

    self.revert_damage_factor_hook = self:add_update_hook (
	secs_to_ticks (30),
	false,
	function (self)
	    self.damage_factor = 1
	    return false
	end
    )

    function self:get_bloodlust_hook ()
	self.damage_factor = 2
	self:enable_update_hook (self.revert_damage_factor_hook)
    end

    -- fire hook

    function self:_internal_fire_hook ()
	if self.can_fire == false then
	    return
	elseif not self:has_ammo_for (self.current_weapon) then
	    self:auto_switch_weapon ()
	    return
	end

	-- have ammo, will fire
	local proj = self.current_weapon.fire (self)

	if self.damage_factor ~= 1 then
	    if type (proj) == "table" then
		for _, p in proj do
		    p.damage = p.damage * self.damage_factor
		end
	    else
		proj.damage = proj.damage * self.damage_factor
	    end
	end

	call_method_on_clients (self, "start_firing_anim")
    end


    --------------------------------------------------
    --  Mines stuff
    --------------------------------------------------

    -- don't use the _ammo variable so that mines are not counted in
    -- backpacks
    self.num_mines = 0		

    function self:_internal_drop_mine_hook ()
	if self.num_mines > 0 then
	    self.num_mines = self.num_mines - 1
	    local obj = spawn_object ("basic-mine-dropped", self.x, self.y)
	    obj.owner = self.id
	end
    end

    function self:receive_mines (amount)
	self.num_mines = self.num_mines + amount
    end


    --------------------------------------------------
    --  Health (and armour) stuff
    --------------------------------------------------

    self.health = 100
    _internal_tell_health (self, self.health)

    self.armour = 0
    _internal_tell_armour (self, self.armour)

    function self:receive_damage (damage, attacker, collision_x, collision_y)
	if self.health <= 0 then
	    -- This is to prevent a bug that we only saw once.
	    -- A player managed to die three times at once.
	    return
	end

	spawn_blood_on_clients (collision_x, collision_y, damage * 100, 2)
	if damage/2 >= 1 then
	    spawn_blod_on_clients (self.x, self.y, damage/2)
	end

	local dmg = damage/2	-- armour absorbs half damage
	self.armour = self.armour - dmg
	if self.armour < 0 then
	    dmg = dmg + -self.armour
	    self.armour = 0
	end
	self.health = self.health - dmg

	_internal_tell_health (self, self.health)
	_internal_tell_armour (self, self.armour)

	if self.health <= 0 then

	    -- We're dead

	    -- give the attacker points, or whatever
	    if attacker then
		_internal_player_died_hook (self.id, attacker)
	    end

	    -- spawn a corpse
	    local i = math.random (table.getn (corpses))
	    local corpse = spawn_object (corpses[i], self.x, self.y)
	    if corpse then
		-- this makes the client track the corpse
		corpse._internal_stalk_me = self.id
		corpse:add_creation_field ("_internal_stalk_me")

		-- drop a backpack after the corpse finishes animation
		local x, y = self.x, self.y	     -- must do this
		local ammo, id = self._ammo, self.id
		corpse.drop_backpack = function ()
		    local backpack = spawn_object ("basic-backpack", x, y)
		    backpack.ammo = ammo
		    backpack.original_owner = id
		end
	    end

	    -- play a sound
	    local i = math.random (table.getn (death_sounds))
	    play_sound_on_clients (corpse, death_sounds[i])

	    -- destroy the object
	    self:set_stale ()

	else

	    -- not dead: make some pretty graphics
	    call_method_on_clients (self, "get_hurt_hook")

	end
    end

    function self:receive_health (amount)
	local h = math.min (100, self.health + amount)
	if h ~= self.health then
	    self.health = h
	    _internal_tell_health (self, self.health)
	    return true
	else
	    return false
	end
    end

    function self:receive_armour (amount)
	local a = math.min (50, self.armour + amount)
	if a ~= self.armour then
	    self.armour = a
	    _internal_tell_armour (self, self.armour)
	    return true
	else
	    return false
	end
    end


    --------------------------------------------------
    --  Blood trails
    --------------------------------------------------

    self:add_update_hook (
	secs_to_ticks (1),
	true,
	function (self)
	    if self.health <= 20 then
		spawn_blood_on_clients (self.x, self.y, 80, 2)
	    end
	end
    )


    --------------------------------------------------
    --  Dunno stuff
    --------------------------------------------------

    -- initial weapon
    self:receive_weapon ("basic-blaster")

    -- create a respawning ball where the player is
    spawn_object ("basic-respawning-ball", self.x, self.y)

end



------------------------------------------------------------
--  Proxy player object
------------------------------------------------------------


local walk_anim = {
    "/basic/player/walk/000",
    "/basic/player/walk/001",
    "/basic/player/walk/002",
    "/basic/player/walk/003",
    "/basic/player/walk/004",
    "/basic/player/walk/005",
    "/basic/player/walk/006",
    "/basic/player/walk/007"
}


local player_proxy_init = function (self)

    install_multiple_update_hook_system (self, player_update_hook_speed)


    --------------------------------------------------
    -- Some properties
    --------------------------------------------------

    self.xv_decay = xv_decay
    self.yv_decay = yv_decay


    --------------------------------------------------
    --  Layers
    --------------------------------------------------

    self:move_layer (0, cx, cy)
    self.arm_layer = self:add_layer ("/basic/weapon/blaster/1arm000", 0, 3)
    self.arm_frame = 1


    --------------------------------------------------
    --  Lighting stuff
    --------------------------------------------------

    self:add_light (self.is_local and "/basic/light/white-64" or "/basic/light/white-32", 0, 0)

    self.current_light_priority = 0

    self.restore_lighting_hook = self:add_update_hook (
	0,			-- dummy
	false,			-- initially off
	function (self)
	    self:replace_light (0, "/basic/light/white-64", 0, 0)
	end
    )

    function self:change_light (light, timeout, priority)
	if self.is_local then
	    if priority >= self.current_light_priority then
		self:replace_light (0, light, 0, 0)
		self.current_light_priority = priority
		self:adjust_update_hook_speed (self.restore_lighting_hook, timeout)
		self:enable_update_hook (self.restore_lighting_hook)
	    end
	end
    end


    --------------------------------------------------
    --  Highlighting stuff
    --------------------------------------------------

    self.restore_unhighlighting_hook = self:add_update_hook (
	10,
	false,			-- initially off
	function (self)
	    self:set_highlighted (false)
	end
    )

    function self:go_highlighted () -- XXX rename
	self:set_highlighted (true)
	self:enable_update_hook (self.restore_unhighlighting_hook)
    end

    
    --------------------------------------------------
    --  Walking and aiming animation hooks
    --------------------------------------------------

    self.walk_frame = 1

    self:add_update_hook (
	2,
	true,

	function (self)

	    -- walking
	    if _internal_object_moving_horizontally (self) then
		self.walk_frame = self.walk_frame + 1
		if self.walk_frame > table.getn (walk_anim) then
		    self.walk_frame = 1
		end
		self:replace_layer (0, walk_anim[self.walk_frame], cx, cy)
	    end

	    -- aiming
	    if self.last_aim_angle ~= self.aim_angle then
		self.last_aim_angle = self.aim_angle

		local angle = radian_to_bangle (self.aim_angle)
		local hflip = (angle < -63 or angle > 63)
		self:hflip_layer (0, hflip)
		self:hflip_layer (self.arm_layer, hflip)
		self:rotate_layer (self.arm_layer, angle - (hflip and 128 or 0))
	    end

	end
    )


    --------------------------------------------------
    --  Firing animation hook
    --------------------------------------------------

    self.arm_animation_hook = self:add_update_hook (
	0,			-- dummy
	false,			-- initially off
	function (self)
	    -- this situation can arise due to network conditions
	    if not self.current_weapon then return end

	    local anim = self.current_weapon.arm_anim
	    self.arm_frame = self.arm_frame + 1
	    if self.arm_frame > table.getn (self.current_weapon.arm_anim) then
		self.arm_frame = 1
		self:replace_layer (self.arm_layer, anim[self.arm_frame], anim.cx, anim.cy)
		return false  -- end of animation
	    else
		self:replace_layer (self.arm_layer, anim[self.arm_frame], anim.cx, anim.cy)
	    end
	end
    )    


    --------------------------------------------------
    --  Hooks
    --------------------------------------------------

    -- (called by nonproxy fire hook)
    function self:start_firing_anim ()
	if self.current_weapon then
	    self:adjust_update_hook_speed (self.arm_animation_hook,
					   self.current_weapon.arm_anim.tics or 5)
	    self:enable_update_hook (self.arm_animation_hook)
	    if self.current_weapon.sound then
		play_sound (self, self.current_weapon.sound)
	    end
	end
    end

    -- (called by nonproxy receive_weapon)
    self._internal_draw_weapons_list = {}
    function self:get_new_weapon_hook (weapon_name)
	if self.is_local then
	    local w = weapons[weapon_name]
	    table.insert (self._internal_draw_weapons_list, w)
	    table.sort (self._internal_draw_weapons_list,
			function (w1, w2)
			    return (index_of (weapon_order, w1.name) < 
				    index_of (weapon_order, w2.name))
			end)
	end
    end

    -- (called by nonproxy switch_weapon)
    function self:switch_weapon_hook (weapon_name)
	local w = weapons[weapon_name]
	self.current_weapon = w
	self:replace_layer (self.arm_layer, w.arm_anim[1], w.arm_anim.cx, w.arm_anim.cy)
	if self.is_local then
	    _internal_set_camera (false, 96)
	    if w.client_switch_to_hook then
		w.client_switch_to_hook ()
	    end
	end
    end

    -- (called by nonproxy receive damage function)
    function self:get_hurt_hook ()
	if self.is_local then
	    self:change_light ("/basic/light/red-64", 38, 10)
	else
	    self:go_highlighted ()
	end
    end

    -- (called by server when player gets lightamp)
    function self:get_lightamp_hook ()
	if self.is_local then
	    self:change_light ('/basic/powerup/lightamp/light', 3000, 20)
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
	anim[i] = string.format (t.fmt, i)
    end

    -- Note: both proxy and nonproxy have to update self.frames.

    local nonproxy_update = function (self)
	self.frame = self.frame + 1
	if self.frame > t.frames then
	    self.frame = t.frames
	    if self.drop_backpack then
		-- drop the backpack after a little while
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

    table.insert (corpses, t.name)

    return Objtype (t, {
    
	icon = anim[0],
	
	nonproxy_init = function (self)
	    if t.mask then
		self:set_mask (mask_main, t.mask, t.cx, t.cy)
		self:set_mask (mask_bottom, t.mask, t.cx, t.cy)
	    end
	    self:set_masks_centre (t.cx, t.cy)
	    self:set_collision_flags ("tn")
	    self.mass = 0.005
	    self.frame = 0
	    self:add_creation_field ("frame")
	    self:set_update_hook (t.speed, nonproxy_update)

	    function self:collide_hook (other)
		-- assume we are getting shot, and spew a bit of blood
		spawn_blood_on_clients (self.x, self.y+5, 10, 1.5)
		return false
	    end
	end,
	
	proxy_init = function (self)
	    -- self.frame is given by server
	    self:add_light ("/basic/light/red-64", 0, 0)
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
	self:set_mask (mask_bottom, "/basic/player/backpack-mask", 6, 8)
	function self:collide_hook (player)
	    -- give goodies
	    if self.ammo then
		for type, amount in self.ammo do
		    player:receive_ammo (type, amount/2)
		end
		if player.id == self.original_owner then
		    send_text_message (player.id, "You got your backpack back")
		else
		    send_text_message (player.id, "You stole a backpack")
		end
	    else
		send_text_message (player.id, "internal error in backpack collide_hook")
	    end
	    self:set_stale ()
	end
    end
}



----------------------------------------------------------------------
--  Death sounds
----------------------------------------------------------------------

death_sounds = {
    "/basic/player/death-sounds/000",
    "/basic/player/death-sounds/001",
    "/basic/player/death-sounds/002",
    "/basic/player/death-sounds/003",
    "/basic/player/death-sounds/004",
    "/basic/player/death-sounds/005",
    "/basic/player/death-sounds/006",
    "/basic/player/death-sounds/007",
    "/basic/player/death-sounds/008",
    "/basic/player/death-sounds/009",
    "/basic/player/death-sounds/010",
    "/basic/player/death-sounds/011"
}
