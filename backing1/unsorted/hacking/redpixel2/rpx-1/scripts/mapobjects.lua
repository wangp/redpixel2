-- scripts/mapobj.lua
-- object editing mode


function button_down(x, y, b)
end


function button_up(x, y, b)
end


function mouse_drag(x, y, b)
end


function mouse_clicked(x, y, b)
end


function palette_key()
end


function palette_click(x, y, b)
end


function objs_draw(x, y)
end


function palette_draw()
end


-- what all that crap amounts to:
mode_objects = {
    switchkey = KEY_2,
    
    mdown = button_down,
    mup = button_up,
    mdrag = mouse_drag,
    mclicked = button_clicked,
	
    pkey = palette_key,
    pclick = palette_click,

    draw = tiles_draw,
    pdraw = palette_draw
}


-- give ourselves up to the great mapedit.lua
register_mode("objects", mode_objects)
