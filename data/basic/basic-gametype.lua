-- basic-gametype.lua


----------------------------------------------------------------------
--  Generic game type handling core
--  (written here to make the C code easier)
----------------------------------------------------------------------

-- The current game type
_internal_game_type = nil

-- Called by server C code to start a game type
function _internal_start_game_type (gt)
    if gt == nil then
	error ("Gametype cannot be nil\n")
    else
	_internal_game_type = gt
	if _internal_game_type.init then
	    _internal_game_type.init ()
	end
    end
end

-- Called by server C code to end the current game type
function _internal_end_game_type ()
    if _internal_game_type then
	if _internal_game_type.shutdown then
	    _internal_game_type.shutdown ()
	end
	_internal_game_type = nil
    end
end

-- 
function _internal_player_spawned_hook (id)
    if _internal_game_type.player_spawned then
	_internal_game_type.player_spawned (id)
    end
end


----------------------------------------------------------------------
--  Deathmatch game type
--  The only game type for now.  Usual deathmatch, you kill, you score.
----------------------------------------------------------------------

local dm_frags

Game_Type_Deathmatch = {
    init =
	function ()
	    dm_frags = {}
	end,

    player_spawned = 
	function (id)
	    if dm_frags[id] == nil then
		dm_frags[id] = 0 
	    end
	end,

    player_died =
	function (player_id, killer_id)
	    if dm_frags[player_id] == nil or dm_frags[killer_id] == nil then
		-- no such player, maybe a dummy object
		return
	    end

	    dm_frags[killer_id] = (dm_frags[killer_id] + 
				   ((player_id == killer_id) and -1 or 1))
 	    set_score (killer_id, tostring (dm_frags[killer_id]))
	end
}
