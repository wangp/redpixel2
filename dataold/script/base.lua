-- base.lua


-- game object base

function object_init (self)
    self.add_layer = object_add_layer
    self.replace_layer = object_replace_layer
    self.move_layer = object_move_layer
    self.remove_layer = object_remove_layer
    self.remove_all_layers = object_remove_all_layers
    self.add_light = object_add_light
    self.replace_light = object_replace_light
    self.move_light = object_move_light
    self.remove_light = object_remove_light
    self.remove_all_lights = object_remove_all_lights
    self.set_mask = object_set_mask
    self.remove_mask = object_remove_mask
    self.remove_all_masks = object_remove_all_masks
end
