-- basic-base.lua -- XXX: rename this file?


-- constants

mask_main = 0 
mask_top = 1
mask_bottom = 2
mask_left = 3
mask_right = 4


-- game object base

local dummy = function () end

function _internal_object_init_hook (self)

    self.set_update_hook = object_set_update_hook
    self.remove_update_hook = object_remove_update_hook

    if not self.is_proxy then

	-- nonproxy methods
	self.destroy = object_destroy
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

	-- proxy methods
	self.add_layer = dummy
	self.replace_layer = dummy
	self.move_layer = dummy
	self.rotate_layer = dummy
	self.remove_layer = dummy
	self.remove_all_layers = dummy
	self.add_light = dummy
	self.replace_light = dummy
	self.move_light = dummy
	self.remove_light = dummy
	self.remove_all_lights = dummy

    else

	-- nonproxy methods
	self.destroy = dummy
	self.hide = dummy
	self.show = dummy
	self.set_collision_flags = dummy
	self.add_creation_field = dummy
	self.set_mask = dummy
	self.set_masks_centre = dummy
	self.remove_mask = dummy
	self.remove_all_masks = dummy
	self.receive_damage = dummy

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

    end
end


-- objtype_register wrapper

function Objtype (t)
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
    end
end


-- weapon registry

weapons = {}

function Weapon (t)
    weapons[t.name] = t
end


-- utility functions

function radian_to_bangle (rads)
    return rads * 128 / PI
end

function merge (t1, t2)
    local t = {}
    for i,v in t1 do t[i] = v end
    for i,v in t2 do t[i] = v end
    return t
end
