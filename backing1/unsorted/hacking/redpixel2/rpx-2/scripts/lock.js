/* Locking mechanisms
 *
 * Peter Wang <tjaden@psynet.net>
 */

var __rpxedit__locks = [];

function lock(key) {
    if (__rpxedit__locks[key])
	return false;
    __rpxedit__locks[key] = true; 
    return true;
}

function unlock(key) { delete __rpxedit__locks[key]; }

function locked(key) { return __rpxedit__locks[key]; }
