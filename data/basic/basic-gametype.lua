-- basic-gametype.lua


----------------------------------------------------------------------
--  Generic game type handling core
--  (written here to make the C code easier)
----------------------------------------------------------------------

-- The current game type
local game_type = nil

-- Called by server C code at the start of a new session
-- gt is the game type to use
function _internal_new_session_hook (gt)
    if type (gt) ~= "table" then
	error ("Gametype not a table\n")
    else
	game_type = gt
	if game_type.new_session then
	    game_type.new_session ()
	end
    end
end

-- Called by server C code when a new map is loaded
function _internal_new_map_hook (map_name)
    if game_type.new_map then
	game_type.new_map (map_name)
    end
end

-- Called by server C code when a player is spawned
function _internal_player_spawned_hook (id)
    if game_type.player_spawned then
	game_type.player_spawned (id)
    end
end

-- Called by player Lua code when a player is killed
function _internal_player_died_hook (player_id, killer_id)
    if game_type.player_died then
	game_type.player_died (player_id, killer_id)
    end
end


----------------------------------------------------------------------
--  Deathmatch game type
--
--  The only game type for now.  Usual deathmatch, you kill, you score.
--
--  For a minor difference, we track the total number of frags per
--  player, as well as frags scored in the current map.
----------------------------------------------------------------------

-- XXX: this code will be wrong if the client id numbers wrap around
-- after 1023 and get reused.  That's not going to happen..

local dm_frags_total
local dm_frags_current_map

local set_deathmatch_score =
function (id)
    local sc
    if dm_frags_current_map[id] ~= dm_frags_total[id] then
	sc = (dm_frags_total[id].." ("..dm_frags_current_map[id]..")")
    else
	sc = tostring (dm_frags_total[id])
    end
    set_score (id, sc)
end

Game_Type_Deathmatch = {

    new_session =
	function ()
	    dm_frags_total = {}
	end,

    new_map =
	function (map_name)
	    dm_frags_current_map = {}
	end,

    player_spawned = 
	function (id)
	    if dm_frags_total[id] == nil then
		dm_frags_total[id] = 0
	    end
	    if dm_frags_current_map[id] == nil then
		dm_frags_current_map[id] = 0
		set_deathmatch_score (id)
	    end
	end,

    player_died =
	function (player_id, killer_id)
	    if (dm_frags_total[player_id] ~= nil and
		dm_frags_total[killer_id] ~= nil) then
		local kid = killer_id
		local d = (player_id == kid) and -1 or 1

		dm_frags_total[kid] = dm_frags_total[kid] + d
		dm_frags_current_map[kid] = dm_frags_current_map[kid] + d
		set_deathmatch_score (kid)

		if d < 0 then
		    broadcast_text_message (get_client_name (player_id)
					    .." committed suicide")
		else
		    broadcast_text_message (get_client_name (player_id)
					    .." was killed by "..
					    get_client_name (killer_id))
		end
	    end
	end
}
