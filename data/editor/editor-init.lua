-- editor-init.lua

store_load ("editor/editor-support.dat", "/editor/")

-- Some functions cannot be run while in the editor or it will crash.
function spawn_blood () end
function spawn_blod () end
function call_method_on_clients () end
