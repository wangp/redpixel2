/*
** Lua binding: red
** Generated automatically by tolua 3.0b on Sun Jun 27 18:39:06 1999.
*/

/* Exported function: package initialization */
int tolua_red_open (void);

#include "tolua.h"

#include <lua.h>
#include <allegro.h>
#include "tiles.h"
#include "weapon.h"
#include "object.h"
#include "scripthelp.h"
#include "mapeditor.h"
#include "rpxfiles.h"
#include "tiles.h"
#include "defs.h"

/* tag variables */
static int tolua_tag_BITMAP;
static int tolua_tag_DATAFILE;

/* function to register type and initialize tag */
static void toluaI_init_tag (void)
{
 tolua_usertype("BITMAP");
 tolua_usertype("DATAFILE");
 tolua_settag("BITMAP",&tolua_tag_BITMAP);
 tolua_settag("DATAFILE",&tolua_tag_DATAFILE);
}

/* function: add_tiles_pack */
static void toluaI_red_add_tiles_pack0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  {
   add_tiles_pack(filename);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'add_tiles_pack'.");
}

/* function: tile_bmp */
static void toluaI_red_tile_bmp0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int i = ((int)  tolua_getnumber(1,0));
  {
   BITMAP* toluaI_ret = tile_bmp(i);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'tile_bmp'.");
}

/* function: register_weapon_type */
static void toluaI_red_register_weapon_type0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* name = ((char*)  tolua_getstring(1,0));
  lua_Object table = ((lua_Object)  tolua_getobject(2,0));
  {
   register_weapon_type(name,table);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'register_weapon_type'.");
}

/* function: register_object_type */
static void toluaI_red_register_object_type0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* name = ((char*)  tolua_getstring(1,0));
  lua_Object table = ((lua_Object)  tolua_getobject(2,0));
  BITMAP* icon = ((BITMAP*)  tolua_getusertype(3,0));
  {
   register_object_type(name,table,icon);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'register_object_type'.");
}

/* function: reset_rpx */
static void toluaI_red_reset_rpx0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   reset_rpx();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'reset_rpx'.");
}

/* function: load_rpx */
static void toluaI_red_load_rpx0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  {
   int toluaI_ret = load_rpx(filename);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_rpx'.");
}

/* function: save_rpx */
static void toluaI_red_save_rpx0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  {
   int toluaI_ret = save_rpx(filename);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'save_rpx'.");
}

/* function: __script_help__key_shifts */
static void toluaI_red_key_shifts0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int flag = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = __script_help__key_shifts(flag);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'key_shifts'.");
}

/* function: __script_help__df */
static void toluaI_red_df0(void)
{
 if (
     !tolua_istype(1,tolua_tag_DATAFILE,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  DATAFILE* d = ((DATAFILE*)  tolua_getusertype(1,0));
  int i = ((int)  tolua_getnumber(2,0));
  {
   void* toluaI_ret = __script_help__df(d,i);
   tolua_pushuserdata(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'df'.");
}

/* function: __script_help__df_item_num */
static void toluaI_red_df_item_num0(void)
{
 if (
     !tolua_istype(1,tolua_tag_DATAFILE,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  DATAFILE* dat = ((DATAFILE*)  tolua_getusertype(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  {
   int toluaI_ret = __script_help__df_item_num(dat,name);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'df_item_num'.");
}

/* function: __script_help__df_item */
static void toluaI_red_df_item0(void)
{
 if (
     !tolua_istype(1,tolua_tag_DATAFILE,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  DATAFILE* dat = ((DATAFILE*)  tolua_getusertype(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  {
   void* toluaI_ret = __script_help__df_item(dat,name);
   tolua_pushuserdata(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'df_item'.");
}

/* function: __script_help__df_count */
static void toluaI_red_df_count0(void)
{
 if (
     !tolua_istype(1,tolua_tag_DATAFILE,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  DATAFILE* dat = ((DATAFILE*)  tolua_getusertype(1,0));
  int type = ((int)  tolua_getnumber(2,0));
  {
   int toluaI_ret = __script_help__df_count(dat,type);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'df_count'.");
}

/* function: __script_help__bmp_line */
static void toluaI_red_bmp_line0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  unsigned int row = ((unsigned int)  tolua_getnumber(2,0));
  {
   unsigned char* toluaI_ret = __script_help__bmp_line(bmp,row);
   tolua_pushstring(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'bmp_line'.");
}

/* function: __script_help__bmp_w */
static void toluaI_red_bmp_w0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* b = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = __script_help__bmp_w(b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'bmp_w'.");
}

/* function: __script_help__bmp_h */
static void toluaI_red_bmp_h0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* b = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = __script_help__bmp_h(b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'bmp_h'.");
}

/* function: __script_help__get_tile */
static void toluaI_red_get_tile0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int x = ((int)  tolua_getnumber(1,0));
  int y = ((int)  tolua_getnumber(2,0));
  {
   unsigned short toluaI_ret = __script_help__get_tile(x,y);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_tile'.");
}

/* function: __script_help__set_tile */
static void toluaI_red_set_tile0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  int x = ((int)  tolua_getnumber(1,0));
  int y = ((int)  tolua_getnumber(2,0));
  int t = ((int)  tolua_getnumber(3,0));
  {
   __script_help__set_tile(x,y,t);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_tile'.");
}

/* function: __script_help__BITMAP */
static void toluaI_red_BITMAP0(void)
{
 if (
     !tolua_istype(1,tolua_tag_userdata,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  void* d = ((void*)  tolua_getuserdata(1,0));
  {
   BITMAP* toluaI_ret = __script_help__BITMAP(d);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'BITMAP'.");
}

/* get function: editbuf */
static void toluaI_get_map_editbuf(void)
{
  tolua_pushusertype((void*)editbuf,tolua_tag_BITMAP);
}

/* set function: editbuf */
static void toluaI_set_map_editbuf(void)
{
  if (!tolua_istype(1,tolua_tag_BITMAP,0))
   tolua_error("#vinvalid type in variable assignment.");
  editbuf = ((BITMAP*)  tolua_getusertype(1,0));
}

/* get function: editbuf_w */
static void toluaI_get_map_editbuf_w(void)
{
  tolua_pushnumber((double)editbuf_w);
}

/* set function: editbuf_w */
static void toluaI_set_map_editbuf_w(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  editbuf_w = ((int)  tolua_getnumber(1,0));
}

/* get function: editbuf_h */
static void toluaI_get_map_editbuf_h(void)
{
  tolua_pushnumber((double)editbuf_h);
}

/* set function: editbuf_h */
static void toluaI_set_map_editbuf_h(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  editbuf_h = ((int)  tolua_getnumber(1,0));
}

/* get function: palbuf */
static void toluaI_get_map_palbuf(void)
{
  tolua_pushusertype((void*)palbuf,tolua_tag_BITMAP);
}

/* set function: palbuf */
static void toluaI_set_map_palbuf(void)
{
  if (!tolua_istype(1,tolua_tag_BITMAP,0))
   tolua_error("#vinvalid type in variable assignment.");
  palbuf = ((BITMAP*)  tolua_getusertype(1,0));
}

/* get function: palbuf_w */
static void toluaI_get_map_palbuf_w(void)
{
  tolua_pushnumber((double)palbuf_w);
}

/* set function: palbuf_w */
static void toluaI_set_map_palbuf_w(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  palbuf_w = ((int)  tolua_getnumber(1,0));
}

/* get function: palbuf_h */
static void toluaI_get_map_palbuf_h(void)
{
  tolua_pushnumber((double)palbuf_h);
}

/* set function: palbuf_h */
static void toluaI_set_map_palbuf_h(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  palbuf_h = ((int)  tolua_getnumber(1,0));
}

/* get function: palbuf_update */
static void toluaI_get_map_palbuf_update(void)
{
  tolua_pushnumber((double)palbuf_update);
}

/* set function: palbuf_update */
static void toluaI_set_map_palbuf_update(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  palbuf_update = ((int)  tolua_getnumber(1,0));
}

/* Initialization function */
int tolua_red_open (void)
{
 tolua_open();
 toluaI_init_tag();
 tolua_constant(NULL,"TILE_W",TILE_W);
 tolua_constant(NULL,"TILE_H",TILE_H);
 tolua_constant(NULL,"RPX_MAX_WIDTH",RPX_MAX_WIDTH);
 tolua_constant(NULL,"RPX_MAX_HEIGHT",RPX_MAX_HEIGHT);
 tolua_constant(NULL,"TILE_BLANK",TILE_BLANK);
 tolua_module("red");
 tolua_function("red","add_tiles_pack",toluaI_red_add_tiles_pack0);
 tolua_function("red","tile_bmp",toluaI_red_tile_bmp0);
 tolua_function("red","register_weapon_type",toluaI_red_register_weapon_type0);
 tolua_function("red","register_object_type",toluaI_red_register_object_type0);
 tolua_function("red","reset_rpx",toluaI_red_reset_rpx0);
 tolua_function("red","load_rpx",toluaI_red_load_rpx0);
 tolua_function("red","save_rpx",toluaI_red_save_rpx0);
 tolua_function("red","key_shifts",toluaI_red_key_shifts0);
 tolua_function("red","df",toluaI_red_df0);
 tolua_function("red","df_item_num",toluaI_red_df_item_num0);
 tolua_function("red","df_item",toluaI_red_df_item0);
 tolua_function("red","df_count",toluaI_red_df_count0);
 tolua_function("red","bmp_line",toluaI_red_bmp_line0);
 tolua_function("red","bmp_w",toluaI_red_bmp_w0);
 tolua_function("red","bmp_h",toluaI_red_bmp_h0);
 tolua_function("red","get_tile",toluaI_red_get_tile0);
 tolua_function("red","set_tile",toluaI_red_set_tile0);
 tolua_function("red","BITMAP",toluaI_red_BITMAP0);
 tolua_module("map");
 tolua_tablevar("map","editbuf",toluaI_get_map_editbuf,toluaI_set_map_editbuf);
 tolua_tablevar("map","editbuf_w",toluaI_get_map_editbuf_w,toluaI_set_map_editbuf_w);
 tolua_tablevar("map","editbuf_h",toluaI_get_map_editbuf_h,toluaI_set_map_editbuf_h);
 tolua_tablevar("map","palbuf",toluaI_get_map_palbuf,toluaI_set_map_palbuf);
 tolua_tablevar("map","palbuf_w",toluaI_get_map_palbuf_w,toluaI_set_map_palbuf_w);
 tolua_tablevar("map","palbuf_h",toluaI_get_map_palbuf_h,toluaI_set_map_palbuf_h);
 tolua_tablevar("map","palbuf_update",toluaI_get_map_palbuf_update,toluaI_set_map_palbuf_update);
 return 1;
}
