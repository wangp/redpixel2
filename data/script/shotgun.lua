-- shotgun.lua

shotgun_shell_init = function (self)
    self.mass = 1
end

store_load ("object/shotgun.dat", "/weapon/shotgun/")
objtype_register ("item", "shotgun", "/weapon/shotgun/pickup")
objtype_register ("item", "shotgun-shell", "/weapon/shotgun/ammo", shotgun_shell_init)
