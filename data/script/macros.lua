-- macros.lua

function cl ()
    drv = 4; connect ("127.0.0.1")
end

function srv ()
    start_server ()
    map ("test.pit")
end

function sg() start_game() end


function q() quit() end