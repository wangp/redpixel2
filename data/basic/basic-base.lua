-- basic-base.lua -- XXX: rename this file?


-- constants

mask_main = 0 
mask_top = 1
mask_bottom = 2
mask_left = 3
mask_right = 4


-- game object base

local dummy = function () end

function _object_init_hook (self)
    if not self.is_proxy then
	self.destroy = object_destroy
	self.set_collision_flags = object_set_collision_flags
	self.set_mask = object_set_mask
	self.remove_mask = object_remove_mask
	self.remove_all_masks = object_remove_all_masks

	self.add_layer = %dummy
	self.replace_layer = %dummy
	self.move_layer = %dummy
	self.rotate_layer = %dummy
	self.remove_layer = %dummy
	self.remove_all_layers = %dummy
	self.add_light = %dummy
	self.replace_light = %dummy
	self.move_light = %dummy
	self.remove_light = %dummy
	self.remove_all_lights = %dummy
    else
	self.destroy = %dummy
	self.set_collision_flags = %dummy
	self.set_mask = %dummy
	self.remove_mask = %dummy
	self.remove_all_masks = %dummy

	self.add_layer = object_add_layer
	self.replace_layer = object_replace_layer
	self.move_layer = object_move_layer
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
    objtype_register (t.category, t.name, t.icon,
	function (self)
	    if self.is_proxy then
		if %t.proxy_init then %t.proxy_init (self) end
	    else
		if %t.nonproxy_init then %t.nonproxy_init (self) end
	    end
	end)
end


-- weapon registry

weapons = {}

function Weapon (t)
    weapons[t.name] = t
end
