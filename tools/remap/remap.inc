/* generated from remap.lua */

static const char *remap_script = "
-- remap.lua


function pack_fputs_nl (str, f)
    return pack_fputs (str, f) and pack_putc (strbyte ('\\n', 1), f)
end

function ID (a, b, c, d)
    a,b,c,d = strbyte (a, 1), strbyte (b, 1), strbyte (c, 1), strbyte (d, 1)
    return a + shl (b, 8) + shl (c, 16) + shl (d, 24)
end

HEADER_MAGIC		= ID ('p', 'i', 't', 'r')
HEADER_VERSION_0x0100	= 256	-- 0x0100 in decimal
HEADER_SILLY		= \"God... root... what is difference?\"

MARK_TILES		= ID ('t', 'i', 'l', 'e')
MARK_LIGHTS		= ID ('l', 'i', 't', 'e')
MARK_OBJECTS		= ID ('o', 'b', 'j', 'c')
MARK_STARTS		= ID ('s', 't', 'r', 't')

TILE_NIL		= \"()\"


-- version 0x0100 loader

function map_load_0x0100 (filename)
    local map = {}

    local f = pack_fopen (filename, \"rp\")
    if not f then
	return nil
    end

    -- Header.
    local magic = pack_igetl (f)
    local version = pack_igetl (f)
    local tmp = pack_fgets (f)	-- HEADER_SILLY

    if (magic ~= HEADER_MAGIC) or (version ~= HEADER_VERSION_0x0100) then
	return nil
    end
    
    -- Map dimensions.
    
    map.width = pack_igetw (f)
    map.height = pack_igetw (f)

    -- Read chunks.
    while not pack_feof (f) do
    	local chunk = pack_igetl (f)

	if chunk == MARK_TILES then

	    map.tiles = {}

	    for y = 0, map.height-1 do
		map.tiles[y] = {}
		for x = 0, map.width-1 do
		    map.tiles[y][x] = pack_fgets (f)
		end
	    end

	elseif chunk == MARK_LIGHTS then
		
	    local num = pack_igetl (f)

	    map.lights = {}

	    for i = 1, num do
		local x = pack_igetl (f)
		local y = pack_igetl (f)
		local tmp = pack_fgets (f)
		tinsert (map.lights, { type = tmp, x = x, y = y })
	    end

	    map.lights.n = nil

	elseif chunk == MARK_OBJECTS then

	    local num = pack_igetl (f)

	    map.objects = {}

	    for i = 1, num do
		local tmp = pack_fgets (f)
		local x = pack_igetl (f)
		local y = pack_igetl (f)
		tinsert (map.objects, { type = tmp, x = x, y = y })
	    end

	    map.objects.n = nil

	elseif chunk == MARK_STARTS then

	    local num = pack_igetl (f)

	    map.starts = {}

	    for i = 1, num do
		local x = pack_igetl (f)
		local y = pack_igetl (f)
		tinsert (map.starts, { x = x, y = y })
	    end

	    map.starts.n = nil

	else
	    return nil
	end
    end

    pack_fclose (f)

    return map
end


-- version 0x0100 saver

function map_save_0x0100 (filename, map)
    local f = pack_fopen (filename, \"wp\")
    if not f then return nil end

    -- Header. 
    if not pack_iputl (HEADER_MAGIC, f) 
    or not pack_iputl (HEADER_VERSION_0x0100, f)
    or not pack_fputs_nl (HEADER_SILLY, f)
    then return nil end

    -- Map dimensions.
    if not pack_iputw (map.width, f) 
    or not pack_iputw (map.height, f)
    then return nil end
 
    -- Tiles.
    if not pack_iputl (MARK_TILES, f)
    then return nil end

    for _, row in map.tiles do
	for _, tile in row do
	    if not pack_fputs_nl (tile, f) 
	    then return nil end
	end
    end

    -- Lights.
    if map.lights then
	local n = getn (map.lights); map.lights.n = nil

	if not pack_iputl (MARK_LIGHTS, f) 
	or not pack_iputl (n, f) 
	then return nil end

	for i,v in map.lights do
	    if not pack_iputl (v.x, f)
	    or not pack_iputl (v.y, f) 
	    or not pack_fputs_nl (v.type, f) 
	    then return nil end
	end
    end
    
    -- Objects.
    if map.objects then
	local n = getn (map.objects); map.objects.n = nil

	if not pack_iputl (MARK_OBJECTS, f)
	or not pack_iputl (n, f) 
	then return nil end
	
	for i,v in map.objects do
	    if not pack_fputs_nl (v.type, f)
	    or not pack_iputl (v.x, f)
	    or not pack_iputl (v.y, f)
	    then return nil end
	end
    end

    --- Starts.
    if map.starts then
	local n = getn (map.starts); map.starts.n = nil

	if not pack_iputl (MARK_STARTS, f)
	or not pack_iputl (n, f) 
	then return nil end 

	for i,v in map.starts do
	    if not pack_iputl (v.x, f)
	    or not pack_iputl (v.y, f)
	    then return nil end
	end
    end

    pack_fclose (f)
    return 1
end


-- do remapping using current *_mappings table

function map_remap (map)
    if tile_mappings then
	for _, row in map.tiles do
	    for i,v in row do
		if tile_mappings[v] then
		    row[i] = tile_mappings[v]
		elseif v ~= TILE_NIL then
		    print (\"Warning: no mapping for tile: \"..v)
		end	
	    end
	end
    end

    if map.objects and object_mappings then
	local new = {}
	for i,v in map.objects do
	    if object_mappings[v.type] then
		local t = map.objects[i]
		t.type = object_mappings[v.type]
		tinsert (new, t)
	    else 
	        print (\"Warning: no mapping for object \"..v.type..'.  Object deleted.')
	    end
	end
	new.n = nil
	map.objects = new
    end

    if map.lights and light_mappings then
	local new = {}
	for i,v in map.lights do
	    if light_mappings[v.type] then
		local t = map.lights[i]
		t.type = light_mappings[v.type]
		tinsert (new, t)
	    else
		print ('Warning: no mapping for light '..v.type..'.  Light deleted.')
	    end
	end
	new.n = nil
	map.lights = new
    end
end


-- generate template

function generate_template (maps)
    local have_tiles = {}
    local have_objects = {}
    local have_lights = {}

    -- collect haves
    for _, map in maps do
	for _, row in map.tiles do
	    for _, tile in row do
		if tile ~= TILE_NIL then have_tiles[tile] = 1 end
	    end
	end

	if map.objects then
	    for i,v in map.objects do have_objects[v.type] = 1 end
	end

	if map.lights then
	    for i,v in map.lights do have_lights[v.type] = 1 end
	end
    end

    -- write output

    function write_mappings (have, name)
	local tmp = {}
	for i,v in have do tinsert (tmp, i) end
	tmp.n = nil
	sort (tmp)
	print (name..\"_mappings = {\")
	for i,v in tmp do
	    print ('  [\"'..v..'\"] = \"\",')
	end
	print (\"}\\n\")
    end

    write_mappings (have_tiles, \"tile\")
    write_mappings (have_objects, \"object\")
    write_mappings (have_lights, \"light\")
end


-- main

function main (args)

    local argc = getn (args)

    if argc == 1 then
	print \"Usage: remap -t input.pit ...\"
	print \"       remap -c input.pit mappings.lua output.pit\"
	exit (1)
    end

    if args[2] == \"-t\" then
	
	if argc == 2 then
	    print \"Missing input map filename.\"; exit (1)
	end
	
	local maps = {}
	for i = 3, argc do
	    local map = map_load_0x0100 (args[i])
	    if not map then
		print (\"Error loading input map: \"..args[i]); exit (1)
	    end
	    tinsert (maps, map)
	end
	maps.n = nil
	generate_template (maps)

    elseif args[2] == \"-c\" then

	if argc ~= 5 then
	    print \"Conversion must have exactly three arguments.\"; exit (1)
	end

	-- load input
	local map = map_load_0x0100 (args[3])
	if not map then
	    print (\"Error loading input map: \"..args[i]); exit (1)
	end

	-- load mappings
	dofile (args[4])
	
	map_remap (map)

	-- write output
	if not map_save_0x0100 (args[5], map) then
	    print (\"Error writing map: \"..args[5]); exit (1)
	end

    else

	print (\"Don't know what to do.  Quitting.\")
	exit (1)

    end
    
end

-- remap.lua ends here
";