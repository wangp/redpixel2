-- scripts/maptiles.lua
-- tile editing for tiles


__maptiles = {
    top = 0,
    selected = 0
}




function button_down(x, y, b)
    local xx, yy;
    xx = x / TILE_W;
    yy = y / TILE_H;
    red.set_tile(xx, yy, __maptiles.selected);
end


function button_up(x, y, b)
end


function mouse_drag(x, y, b)
    local xx, yy;
    xx = x / TILE_W;
    yy = y / TILE_H;
    red.set_tile(xx, yy, __maptiles.selected);
    dirtify(x,y);
end


function button_clicked(x, y, b)
end


function palette_key()
    if key(KEY_W) ~= 0 and __maptiles.top > 1 then
        __maptiles.top = __maptiles.top - 2;
	updatepal = not nil;
    end
    
    if key(KEY_S) ~= 0 then             -- FIXME: bounds check?
        __maptiles.top = __maptiles.top + 2;
	updatepal = not nil;
    end
end


function palette_click(x, y, b)
    local xx, yy;
    xx = x / TILE_W;
    yy = y / TILE_H;
    __maptiles.selected = __maptiles.top + yy * 2 + xx;
end


function tiles_draw(x, y)
    ex = map.editbuf_w / TILE_W + x;
    ey = map.editbuf_h / TILE_H + y;
	
    yy, yoff = y, 0;
    while yy < ey do
    
    	xx, xoff = x, 0;
	while xx < ex do
            t = red.get_tile(xx, yy);
	    if t ~= TILE_BLANK then
		draw_sprite(map.editbuf, red.tile_bmp(t), xoff, yoff);
	    end
	    xx = xx + 1;
	    xoff = xoff + TILE_W;
	end -- xx
		
	yy = yy + 1;
	yoff = yoff + TILE_H;
    end -- yy
end


function palette_draw()
    local t, b = __maptiles.top;
    local yoff, xoff = 0, 0;
    
    repeat
        b = red.tile_bmp(t);
	if b then 
	    draw_sprite(map.palbuf, b, xoff, yoff); 
            t = t + 1;
	else
	    return;
	end
	
	if xoff == TILE_W then
	    xoff = 0;
	    yoff = yoff + TILE_H;
	else
            xoff = TILE_W;
	end
    until yoff >= map.palbuf_h;
end


-- what all that crap amounts to:
mode_tiles = {
    switchkey = KEY_1,
    
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
register_mode("tiles", mode_tiles)
