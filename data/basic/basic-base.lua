-- basic-base.lua


----------------------------------------------------------------------
-- Utility functions
----------------------------------------------------------------------

function radian_to_bangle (rads)
    return rads * 128 / PI
end

-- Merge t1 and t2, t2 taking precedence in case of clashes.
-- For convenience, if t2 is omitted, this is the identity function
-- (i.e. the table returned is NOT a new table.
function merge (t1, t2)
    local t
    if not t2 then
	t = t1
    else
	t = {}
	for i,v in t1 do t[i] = v end
	for i,v in t2 do t[i] = v end
    end
    return t
end

-- Return true if t contains v as a value.
function contains (t, v)
    for _,x in t do
	if x == v then return true end
    end
    return false
end


----------------------------------------------------------------------
-- Constants
----------------------------------------------------------------------

mask_main = 0 
mask_top = 1
mask_bottom = 2
mask_left = 3
mask_right = 4


----------------------------------------------------------------------
-- Game object base
----------------------------------------------------------------------

local dummy = function () end

function _internal_object_init_hook (self)

    self.set_update_hook = object_set_update_hook
    self.remove_update_hook = object_remove_update_hook

    if not self.is_proxy then

	-- nonproxy methods
	self.set_stale = object_set_stale
	self.hide = object_hide
	self.show = object_show
	self.set_collision_flags = object_set_collision_flags
	self.add_creation_field = object_add_creation_field
	self.set_mask = object_set_mask
	self.set_masks_centre = object_set_masks_centre
	self.remove_mask = object_remove_mask
	self.remove_all_masks = object_remove_all_masks
	self.receive_damage = dummy

	function self:hide_and_respawn_later (msecs)
	    self:hide ()
	    self:set_update_hook (msecs,
				  function (self)
				      self:show ()
				      self:remove_update_hook ()
				  end)
	end

    else

	-- proxy methods
	self.add_layer = object_add_layer
	self.replace_layer = object_replace_layer
	self.move_layer = object_move_layer
	self.hflip_layer = object_hflip_layer
	self.rotate_layer = object_rotate_layer
	self.remove_layer = object_remove_layer
	self.remove_all_layers = object_remove_all_layers
	self.add_light = object_add_light
	self.replace_light = object_replace_light
	self.move_light = object_move_light
	self.remove_light = object_remove_light
	self.remove_all_lights = object_remove_all_lights
	self.set_highlighted = object_set_highlighted

    end
end


----------------------------------------------------------------------
-- objtype_register wrapper
----------------------------------------------------------------------

function Objtype (t, u)
    t = merge (t, u)
    local result =
    objtype_register (t.category, t.name, t.icon,
	function (self)
	    if self.is_proxy then
		if t.proxy_init then t.proxy_init (self) end
	    else
		if t.nonproxy_init then t.nonproxy_init (self) end
	    end
	end)
    if not result then
	print ("Objtype not registered properly.")
	return
    end
    if t.alias then
	add_alias (t.name, t.alias)
    end
end


----------------------------------------------------------------------
-- Respawning items wrapper
----------------------------------------------------------------------

store_load ("basic/basic-respawn.dat", "/basic/")

local respawning_ball_anim = {}
local respawning_ball_frames = 19

for i = 0, respawning_ball_frames-1 do
    respawning_ball_anim[i] = format ("/basic/respawning-ball/%03d", i)
end

Objtype {
    name = "basic-respawning-ball",
    alias = "~rb",
    icon = "/basic/respawning-ball/000",
    nonproxy_init = function (self)
	spawn_respawn_particles_on_clients (self.x, self.y, 30, 1.5)
	self:set_collision_flags ("")
	self:set_update_hook (
	    1000,
	    function (self)
		if self.item then
		    self.item:show ()
		end
		self:set_stale ()
	    end
	)
    end,
    proxy_init = function (self)
	self.frame = 0
	self.theta = 0
	self.dtheta = random (8) + 8
	self:add_light ("/basic/respawning-ball/light", 0, 0)
	self:add_light ("/basic/respawning-ball/light", 0, 0)
	self:set_update_hook (
	    1000 / respawning_ball_frames,
	    function (self)
		self.frame = self.frame + 1
		if self.frame >= respawning_ball_frames then
		    self:remove_update_hook ()
		else
		    self:replace_layer (0, respawning_ball_anim[self.frame], 8, 8)
		end
		self.theta = self.theta + self.dtheta
		local dx, dy = cos (self.theta) * 4, sin (self.theta) * 4
		self:move_light (0, dx, dy)
		self:move_light (1, -dx, -dy)
	    end
	)
    end
}

function Respawning_Item (t, u)
    t = merge (t, u)
    return Objtype (t, {
	category = t.category or "item",
	nonproxy_init = function (self)
	    self:set_collision_flags ("p")
	    function self:collide_hook (player)
		if t.collide_hook and t.collide_hook (self, player) == false then
		    return false
		end
		self:hide ()
		self:set_update_hook (
		    1000 * t.respawn_secs,
		    function (self)
			local ball = spawn_object ("basic-respawning-ball",
						   self.x, self.y)
			ball.item = self
			self:remove_update_hook()
		    end
		)
	    end
	end
    })
end


----------------------------------------------------------------------
-- Weapon registry
----------------------------------------------------------------------

weapons = {}

function Weapon (t, u)
    weapons[t.name] = merge (t, u)
end
