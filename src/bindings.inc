/* Generated by bindgen.lua on Thu Apr 26 08:24:11 2001.  Do not edit.  */

static int bind_store_load(lua_State *L)
{
	const char * filename;
	const char * prefix;
	int ret;
	if (!checkargs(L, "ss")) goto error;
	filename = tostring(L, 1);
	prefix = tostring(L, 2);
	ret = store_load_ex(filename, prefix, load_extended_datafile);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_objtype_register(lua_State *L)
{
	const char * type;
	const char * name;
	const char * icon;
	lref_t func;
	if (!checkargs(L, "sss[fN-]")) goto error;
	type = tostring(L, 1);
	name = tostring(L, 2);
	icon = tostring(L, 3);
	func = ((isnil(L, 4) || isnull(L, 4)) ? LUA_NOREF : ref(L, 4));
	objtypes_register(type, name, icon, func);
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_set_mass(lua_State *L)
{
	object_t * obj;
	float mass;
	if (!checkargs(L, "tn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	mass = tonumber(L, 2);
	object_set_mass(obj, mass);
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_set_ramp(lua_State *L)
{
	object_t * obj;
	float rampamount;
	if (!checkargs(L, "tn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	rampamount = tonumber(L, 2);
	object_set_ramp(obj, rampamount);
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_add_layer(lua_State *L)
{
	object_t * obj;
	const char * image;
	int xoffset;
	int yoffset;
	int layerid;
	if (!checkargs(L, "tsnn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	image = tostring(L, 2);
	xoffset = tonumber(L, 3);
	yoffset = tonumber(L, 4);
	layerid = object_add_layer(obj, image, xoffset, yoffset);
	pushnumber(L, layerid); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_replace_layer(lua_State *L)
{
	object_t * obj;
	int layerid;
	const char * image;
	int xoffset;
	int yoffset;
	int ret;
	if (!checkargs(L, "tnsnn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	layerid = tonumber(L, 2);
	image = tostring(L, 3);
	xoffset = tonumber(L, 4);
	yoffset = tonumber(L, 5);
	ret = object_replace_layer(obj, layerid, image, xoffset, yoffset);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_move_layer(lua_State *L)
{
	object_t * obj;
	int layerid;
	int xoffset;
	int yoffset;
	int ret;
	if (!checkargs(L, "tnnn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	layerid = tonumber(L, 2);
	xoffset = tonumber(L, 3);
	yoffset = tonumber(L, 4);
	ret = object_move_layer(obj, layerid, xoffset, yoffset);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_remove_layer(lua_State *L)
{
	object_t * obj;
	int layerid;
	int ret;
	if (!checkargs(L, "tn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	layerid = tonumber(L, 2);
	ret = object_remove_layer(obj, layerid);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_remove_all_layers(lua_State *L)
{
	object_t * obj;
	if (!checkargs(L, "t")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	object_remove_all_layers(obj);
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_add_light(lua_State *L)
{
	object_t * obj;
	const char * image;
	int xoffset;
	int yoffset;
	int lightid;
	if (!checkargs(L, "tsnn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	image = tostring(L, 2);
	xoffset = tonumber(L, 3);
	yoffset = tonumber(L, 4);
	lightid = object_add_light(obj, image, xoffset, yoffset);
	pushnumber(L, lightid); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_replace_light(lua_State *L)
{
	object_t * obj;
	int lightid;
	const char * image;
	int xoffset;
	int yoffset;
	int ret;
	if (!checkargs(L, "tnsnn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	lightid = tonumber(L, 2);
	image = tostring(L, 3);
	xoffset = tonumber(L, 4);
	yoffset = tonumber(L, 5);
	ret = object_replace_light(obj, lightid, image, xoffset, yoffset);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_move_light(lua_State *L)
{
	object_t * obj;
	int lightid;
	int xoffset;
	int yoffset;
	int ret;
	if (!checkargs(L, "tnnn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	lightid = tonumber(L, 2);
	xoffset = tonumber(L, 3);
	yoffset = tonumber(L, 4);
	ret = object_move_light(obj, lightid, xoffset, yoffset);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_remove_light(lua_State *L)
{
	object_t * obj;
	int lightid;
	int ret;
	if (!checkargs(L, "tn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	lightid = tonumber(L, 2);
	ret = object_remove_light(obj, lightid);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_remove_all_lights(lua_State *L)
{
	object_t * obj;
	if (!checkargs(L, "t")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	object_remove_all_lights(obj);
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_set_mask(lua_State *L)
{
	object_t * obj;
	int masknumber;
	const char * mask;
	int xoffset;
	int yoffset;
	int ret;
	if (!checkargs(L, "tnsnn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	masknumber = tonumber(L, 2);
	mask = tostring(L, 3);
	xoffset = tonumber(L, 4);
	yoffset = tonumber(L, 5);
	ret = object_set_mask(obj, masknumber, mask, xoffset, yoffset);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_remove_mask(lua_State *L)
{
	object_t * obj;
	int masknumber;
	int ret;
	if (!checkargs(L, "tn")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	masknumber = tonumber(L, 2);
	ret = object_remove_mask(obj, masknumber);
	if (ret < 0) goto error;
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

static int bind_object_remove_all_masks(lua_State *L)
{
	object_t * obj;
	if (!checkargs(L, "t")) goto error;
	obj = table_object(L, 1);
	if (!obj) goto error;
	object_remove_all_masks(obj);
	pushnumber(L, 1); return 1;
error:
	pushnil(L); return 1;
}

#define DO_REGISTRATION(L)  \
	lregister(L, "store_load", bind_store_load); \
	lregister(L, "objtype_register", bind_objtype_register); \
	lregister(L, "object_set_mass", bind_object_set_mass); \
	lregister(L, "object_set_ramp", bind_object_set_ramp); \
	lregister(L, "object_add_layer", bind_object_add_layer); \
	lregister(L, "object_replace_layer", bind_object_replace_layer); \
	lregister(L, "object_move_layer", bind_object_move_layer); \
	lregister(L, "object_remove_layer", bind_object_remove_layer); \
	lregister(L, "object_remove_all_layers", bind_object_remove_all_layers); \
	lregister(L, "object_add_light", bind_object_add_light); \
	lregister(L, "object_replace_light", bind_object_replace_light); \
	lregister(L, "object_move_light", bind_object_move_light); \
	lregister(L, "object_remove_light", bind_object_remove_light); \
	lregister(L, "object_remove_all_lights", bind_object_remove_all_lights); \
	lregister(L, "object_set_mask", bind_object_set_mask); \
	lregister(L, "object_remove_mask", bind_object_remove_mask); \
	lregister(L, "object_remove_all_masks", bind_object_remove_all_masks);