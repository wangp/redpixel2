-- scripts/mapeditor.lua
-- map editor base


print("[ mapeditor.lua ]");



--
-- globals
--

mx, my, update, updatepal = 0, 0, not nil, not nil;

mouse, oldmouse = { }, { }
quit = nil;

white = makecol(255,255,255);



--
-- helper functions for child scripts
--

function dirtify(x,y)
    update = not nil;
end



--
-- mode tables
--

modetbl = {}

-- register_mode:
--  Adds another editing mode to the array
function register_mode(name, tbl)
    print("Registering mode: " .. name);
    modetbl[name] = tbl;
end



--
-- register editing modes
--

dofile("scripts/maptiles.lua");
dofile("scripts/mapobjects.lua");

curmode = next(modetbl, nil);



--
-- helper functions for 'halcyon'
--

-- askquit: 
--  brings up an alert box with the question
function askquit() 
    -- FIXME
    return not nil;

--    if (alert("Are you sure you", "want to quit?", "", "Yes", "No", 13, 27) == 1) then
--        return not nil;
--    end    
--    return nil;
end


-- askfilename: 
--  brings up a file select box
function askfilename(title, path, ext)
    update = not nil;
    r = file_select(title, path, ext);
    text_mode(-1);
    if r ~= 0 then
        return path;
    else    
        return nil;
    end
end


-- overwrite: 
--  if file already exists, ask if want to overwrite
function overwrite(path) 
    if exists(path) ~= 0 then
        if alert(path .. " already exists!", "Overwrite?", "", "Yes", "No", 13, 27) == 1 then
            return not nil;
	else
	    return nil;
	end
    else
        return not nil;
    end
end


-- load:
--  The "Load .rpx" sequence
function load()
    fn = askfilename("Load .rpx", "", "rpx");
    if fn then red.load_rpx(fn); end
end


-- save:
--  The saving sequence
function save()
    fn = askfilename("Save .rpx", "", "rpx");
    if fn and overwrite(fn) then red.save_rpx(fn); end
end


-- drawstuff:
--  Calls 'draw' for each mode, and then blits editbuf to screen
function drawstuff()
    clear(map.editbuf);
    
    local r,v = next(modetbl, nil);
    while r ~= nil do
        modetbl[r].draw(mx, my);
        r,v = next(modetbl, r);
    end

    textout(map.editbuf, font, mx .. ", " .. my, 0, 0, white);

    show_mouse(NULL);
    blit(map.editbuf, screen, 0, 0, 0, 0, map.editbuf_w, map.editbuf_h);
    show_mouse(screen);
    update = nil;
end


-- drawpalette:
--  Calls palette draw for curmode, then blits it to screen
function drawpalette()
    clear(map.palbuf);
    modetbl[curmode].pdraw();
    
    show_mouse(NULL);
    blit(map.palbuf, screen, 0, 0, map.editbuf_w, 0, map.palbuf_w, map.palbuf_h);
    show_mouse(screen);
    updatepal = nil;
end


-- switchmode:
--  Checks 'switchkeys' to see if curmode needs to be changed
function switchmode()
    local r,v = next(modetbl, nil);
    while r ~= nil do
        if (key(modetbl[r].switchkey) ~= 0) then
	    if curmode ~= r then
    	        curmode = r;
		update, updatepal = not nil, not nil;
                print("mode switched to:", r);
            end
	    r = nil;
	else
	    modetbl[r].draw(mx, my);
            r,v = next(modetbl, r);
	end
    end
end


-- abs:
--  Gives the absolute value
function abs(a)
    if a < 0 then return -a; end
    return a;
end


-- moved:
--  Checks if position A has moved far enough from position B for it to be the
--  start of a drag, etc.
function moved(a, b)
    local threshold = 3;
    
    if abs(a.x - b.x) > threshold or abs(a.y - b.y) > threshold then
        return not nil;
    else
        return nil;
    end
end


mnow = { x, y, b }
mold = { x=0, y=0, b=0 }


while not quit do

    if update then drawstuff(); end
    if updatepal then drawpalette(); end
        
    if red.key_shifts(KB_CTRL_FLAG) ~= 0 then

        if key(KEY_L) ~= 0 then load(); end
        if key(KEY_S) ~= 0 then save(); end

        if key(KEY_Q) ~= 0 and askquit() then 
	    quit = not nil; 
	end
    else
        switchmode();
    end

    mnow.x = mx * TILE_W + mouse_x;
    mnow.y = my * TILE_H + mouse_y;
    mnow.b = mouse_b;

    if mouse_x < map.editbuf_w then
	
        -- WASD scrolling
	
        if key(KEY_W) ~= 0 and my > 0 then my = my - 1; update = not nil; end
        if key(KEY_S) ~= 0            then my = my + 1; update = not nil; end  -- FIXME
        if key(KEY_A) ~= 0 and mx > 0 then mx = mx - 1; update = not nil; end
        if key(KEY_D) ~= 0            then mx = mx + 1; update = not nil; end  -- FIXME
	    
	-- mouse events

        if mnow.b ~= 0 then         -- mouse button is down
	    if mold.b == 0 then
                modetbl[curmode].mdown(mnow.x, mnow.y, mnow.b);
            elseif moved(mnow, mold) then
	        modetbl[curmode].mdrag(mnow.x, mnow.y, mnow.b);
	    end
        elseif mold.b ~= 0 then    -- mouse button *was* down
	    if not moved(mnow, mold) then
	        modetbl[curmode].mclicked(mnow.x, mnow.y, mold.b);
            end

            modetbl[curmode].mup(mnow.x, mnow.y, mnow.b);
	end

        mold.x = mnow.x;
	mold.y = mnow.y;
	mold.b = mnow.b;

    else

        modetbl[curmode].pkey();

        if mnow.b ~= 0 then
            modetbl[curmode].pclick(mnow.x - map.editbuf_w, mnow.y, mnow.b);
	end

    end

end
