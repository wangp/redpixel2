/* cache.js - Almost the same idea as the locks.
 *
 * Peter Wang <tjaden@psynet.net>
 */
 
 
var __rpxedit__cache = [];

function cache_lookup(var_id, default_val) {
    if (__rpxedit__cache[var_id])
	return __rpxedit__cache[var_id];
    return (__rpxedit__cache[var_id] = default_val);
}

function uncache(var_id) { 
    delete __rpxedit__cache[var_id]; 
}

function cache_value(var_id, value) {
    return (__rpxedit__cache[var_id] = value);
}
