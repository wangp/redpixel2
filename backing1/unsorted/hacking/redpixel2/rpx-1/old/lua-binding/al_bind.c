/*
** Lua binding: allegro
** Generated automatically by tolua 3.0b on Tue Jun  1 11:27:00 1999.
*/

/* Exported function: package initialization */
int tolua_allegro_open (void);

#include "tolua.h"

#include <allegro.h>
#include "bindhelp.h"

/* tag variables */
static int tolua_tag_RGB;
static int tolua_tag_BITMAP;
static int tolua_tag_FONT;
static int tolua_tag_SAMPLE;
static int tolua_tag_MIDI;
static int tolua_tag_AUDIOSTREAM;
static int tolua_tag_PACKFILE;
static int tolua_tag_DATAFILE;

/* function to register type and initialize tag */
static void toluaI_init_tag (void)
{
 tolua_usertype("DATAFILE");
 tolua_usertype("SAMPLE");
 tolua_usertype("AUDIOSTREAM");
 tolua_usertype("PACKFILE");
 tolua_usertype("BITMAP");
 tolua_usertype("RGB");
 tolua_usertype("MIDI");
 tolua_usertype("FONT");
 tolua_settag("SAMPLE",&tolua_tag_SAMPLE);
 tolua_settag("MIDI",&tolua_tag_MIDI);
 tolua_settag("AUDIOSTREAM",&tolua_tag_AUDIOSTREAM);
 tolua_settag("FONT",&tolua_tag_FONT);
 tolua_settag("DATAFILE",&tolua_tag_DATAFILE);
 tolua_settag("BITMAP",&tolua_tag_BITMAP);
 tolua_settag("RGB",&tolua_tag_RGB);
 tolua_settag("PACKFILE",&tolua_tag_PACKFILE);
}

/* get function: dat of class  DATAFILE */
static void toluaI_get_DATAFILE_dat(void)
{
  DATAFILE* self = (DATAFILE*)  tolua_getusertype(1,0);
  if (!self) tolua_error("invalid 'self' in accessing variable 'dat'");
  tolua_pushuserdata(self->dat);
}

/* set function: dat of class  DATAFILE */
static void toluaI_set_DATAFILE_dat(void)
{
  DATAFILE* self = (DATAFILE*)  tolua_getusertype(1,0);
  if (!self) tolua_error("invalid 'self' in accessing variable 'dat'");
  if (!tolua_istype(2,tolua_tag_userdata,0))
   tolua_error("#vinvalid type in variable assignment.");
  self->dat = ((void*)  tolua_getuserdata(2,0));
}

/* function: __ugly_but_true__BITMAP */
static void toluaI_BITMAP0(void)
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
   BITMAP* toluaI_ret = __ugly_but_true__BITMAP(d);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'BITMAP'.");
}

/* function: set_config_file */
static void toluaI_set_config_file0(void)
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
   set_config_file(filename);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_config_file'.");
}

/* function: set_config_data */
static void toluaI_set_config_data0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* data = ((char*)  tolua_getstring(1,0));
  int length = ((int)  tolua_getnumber(2,0));
  {
   set_config_data(data,length);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_config_data'.");
}

/* function: override_config_file */
static void toluaI_override_config_file0(void)
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
   override_config_file(filename);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'override_config_file'.");
}

/* function: override_config_data */
static void toluaI_override_config_data0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* data = ((char*)  tolua_getstring(1,0));
  int length = ((int)  tolua_getnumber(2,0));
  {
   override_config_data(data,length);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'override_config_data'.");
}

/* function: push_config_state */
static void toluaI_push_config_state0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   push_config_state();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'push_config_state'.");
}

/* function: pop_config_state */
static void toluaI_pop_config_state0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   pop_config_state();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pop_config_state'.");
}

/* function: get_config_string */
static void toluaI_get_config_string0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_string,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* section = ((char*)  tolua_getstring(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  char* def = ((char*)  tolua_getstring(3,0));
  {
   char* toluaI_ret = get_config_string(section,name,def);
   tolua_pushstring(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_config_string'.");
}

/* function: get_config_int */
static void toluaI_get_config_int0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* section = ((char*)  tolua_getstring(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  int def = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = get_config_int(section,name,def);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_config_int'.");
}

/* function: get_config_hex */
static void toluaI_get_config_hex0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* section = ((char*)  tolua_getstring(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  int def = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = get_config_hex(section,name,def);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_config_hex'.");
}

/* function: get_config_float */
static void toluaI_get_config_float0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* section = ((char*)  tolua_getstring(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  float def = ((float)  tolua_getnumber(3,0));
  {
   float toluaI_ret = get_config_float(section,name,def);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_config_float'.");
}

/* function: set_config_string */
static void toluaI_set_config_string0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_string,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* section = ((char*)  tolua_getstring(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  char* val = ((char*)  tolua_getstring(3,0));
  {
   set_config_string(section,name,val);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_config_string'.");
}

/* function: set_config_int */
static void toluaI_set_config_int0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* section = ((char*)  tolua_getstring(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  int val = ((int)  tolua_getnumber(3,0));
  {
   set_config_int(section,name,val);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_config_int'.");
}

/* function: set_config_hex */
static void toluaI_set_config_hex0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* section = ((char*)  tolua_getstring(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  int val = ((int)  tolua_getnumber(3,0));
  {
   set_config_hex(section,name,val);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_config_hex'.");
}

/* function: set_config_float */
static void toluaI_set_config_float0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* section = ((char*)  tolua_getstring(1,0));
  char* name = ((char*)  tolua_getstring(2,0));
  float val = ((float)  tolua_getnumber(3,0));
  {
   set_config_float(section,name,val);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_config_float'.");
}

/* get function: mouse_x */
static void toluaI_get_mouse_x(void)
{
  tolua_pushnumber((double)mouse_x);
}

/* set function: mouse_x */
static void toluaI_set_mouse_x(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  mouse_x = ((int)  tolua_getnumber(1,0));
}

/* get function: mouse_y */
static void toluaI_get_mouse_y(void)
{
  tolua_pushnumber((double)mouse_y);
}

/* set function: mouse_y */
static void toluaI_set_mouse_y(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  mouse_y = ((int)  tolua_getnumber(1,0));
}

/* get function: mouse_b */
static void toluaI_get_mouse_b(void)
{
  tolua_pushnumber((double)mouse_b);
}

/* set function: mouse_b */
static void toluaI_set_mouse_b(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  mouse_b = ((int)  tolua_getnumber(1,0));
}

/* get function: mouse_pos */
static void toluaI_get_mouse_pos(void)
{
  tolua_pushnumber((double)mouse_pos);
}

/* set function: mouse_pos */
static void toluaI_set_mouse_pos(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  mouse_pos = ((int)  tolua_getnumber(1,0));
}

/* get function: freeze_mouse_flag */
static void toluaI_get_freeze_mouse_flag(void)
{
  tolua_pushnumber((double)freeze_mouse_flag);
}

/* set function: freeze_mouse_flag */
static void toluaI_set_freeze_mouse_flag(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  freeze_mouse_flag = ((int)  tolua_getnumber(1,0));
}

/* function: show_mouse */
static void toluaI_show_mouse0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  {
   show_mouse(bmp);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'show_mouse'.");
}

/* function: position_mouse */
static void toluaI_position_mouse0(void)
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
   position_mouse(x,y);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'position_mouse'.");
}

/* function: set_mouse_range */
static void toluaI_set_mouse_range0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  int x1 = ((int)  tolua_getnumber(1,0));
  int y1 = ((int)  tolua_getnumber(2,0));
  int x2 = ((int)  tolua_getnumber(3,0));
  int y2 = ((int)  tolua_getnumber(4,0));
  {
   set_mouse_range(x1,y1,x2,y2);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_mouse_range'.");
}

/* function: set_mouse_speed */
static void toluaI_set_mouse_speed0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int xspeed = ((int)  tolua_getnumber(1,0));
  int yspeed = ((int)  tolua_getnumber(2,0));
  {
   set_mouse_speed(xspeed,yspeed);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_mouse_speed'.");
}

/* function: set_mouse_sprite */
static void toluaI_set_mouse_sprite0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(1,0));
  {
   set_mouse_sprite(sprite);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_mouse_sprite'.");
}

/* function: set_mouse_sprite_focus */
static void toluaI_set_mouse_sprite_focus0(void)
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
   set_mouse_sprite_focus(x,y);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_mouse_sprite_focus'.");
}

/* function: get_mouse_mickeys */
static void toluaI_get_mouse_mickeys0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int mickeyx = ((int)  tolua_getnumber(1,0));
  int mickeyy = ((int)  tolua_getnumber(2,0));
  {
   get_mouse_mickeys(&mickeyx,&mickeyy);
   tolua_pushnumber((double)mickeyx);
   tolua_pushnumber((double)mickeyy);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_mouse_mickeys'.");
}

/* function: rest */
static void toluaI_rest0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  long time = ((long)  tolua_getnumber(1,0));
  {
   rest(time);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'rest'.");
}

/* function: __ugly_but_true__key */
static void toluaI_key0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int k = ((int)  tolua_getnumber(1,0));
  {
   char toluaI_ret = __ugly_but_true__key(k);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'key'.");
}

/* get function: key_shifts */
static void toluaI_get_key_shifts(void)
{
  tolua_pushnumber((double)key_shifts);
}

/* set function: key_shifts */
static void toluaI_set_key_shifts(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  key_shifts = ((int)  tolua_getnumber(1,0));
}

/* get function: three_finger_flag */
static void toluaI_get_three_finger_flag(void)
{
  tolua_pushnumber((double)three_finger_flag);
}

/* set function: three_finger_flag */
static void toluaI_set_three_finger_flag(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  three_finger_flag = ((int)  tolua_getnumber(1,0));
}

/* get function: key_led_flag */
static void toluaI_get_key_led_flag(void)
{
  tolua_pushnumber((double)key_led_flag);
}

/* set function: key_led_flag */
static void toluaI_set_key_led_flag(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  key_led_flag = ((int)  tolua_getnumber(1,0));
}

/* function: keypressed */
static void toluaI_keypressed0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   int toluaI_ret = keypressed();
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'keypressed'.");
}

/* function: readkey */
static void toluaI_readkey0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   int toluaI_ret = readkey();
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'readkey'.");
}

/* function: simulate_keypress */
static void toluaI_simulate_keypress0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int key = ((int)  tolua_getnumber(1,0));
  {
   simulate_keypress(key);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'simulate_keypress'.");
}

/* function: clear_keybuf */
static void toluaI_clear_keybuf0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   clear_keybuf();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'clear_keybuf'.");
}

/* function: set_leds */
static void toluaI_set_leds0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int leds = ((int)  tolua_getnumber(1,0));
  {
   set_leds(leds);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_leds'.");
}

/* get function: joy_type */
static void toluaI_get_joy_type(void)
{
  tolua_pushnumber((double)joy_type);
}

/* set function: joy_type */
static void toluaI_set_joy_type(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_type = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_x */
static void toluaI_get_joy_x(void)
{
  tolua_pushnumber((double)joy_x);
}

/* set function: joy_x */
static void toluaI_set_joy_x(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_x = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_y */
static void toluaI_get_joy_y(void)
{
  tolua_pushnumber((double)joy_y);
}

/* set function: joy_y */
static void toluaI_set_joy_y(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_y = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_left */
static void toluaI_get_joy_left(void)
{
  tolua_pushnumber((double)joy_left);
}

/* set function: joy_left */
static void toluaI_set_joy_left(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_left = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_right */
static void toluaI_get_joy_right(void)
{
  tolua_pushnumber((double)joy_right);
}

/* set function: joy_right */
static void toluaI_set_joy_right(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_right = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_up */
static void toluaI_get_joy_up(void)
{
  tolua_pushnumber((double)joy_up);
}

/* set function: joy_up */
static void toluaI_set_joy_up(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_up = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_down */
static void toluaI_get_joy_down(void)
{
  tolua_pushnumber((double)joy_down);
}

/* set function: joy_down */
static void toluaI_set_joy_down(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_down = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_b1 */
static void toluaI_get_joy_b1(void)
{
  tolua_pushnumber((double)joy_b1);
}

/* set function: joy_b1 */
static void toluaI_set_joy_b1(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_b1 = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_b2 */
static void toluaI_get_joy_b2(void)
{
  tolua_pushnumber((double)joy_b2);
}

/* set function: joy_b2 */
static void toluaI_set_joy_b2(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_b2 = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_b3 */
static void toluaI_get_joy_b3(void)
{
  tolua_pushnumber((double)joy_b3);
}

/* set function: joy_b3 */
static void toluaI_set_joy_b3(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_b3 = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_b4 */
static void toluaI_get_joy_b4(void)
{
  tolua_pushnumber((double)joy_b4);
}

/* set function: joy_b4 */
static void toluaI_set_joy_b4(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_b4 = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_b5 */
static void toluaI_get_joy_b5(void)
{
  tolua_pushnumber((double)joy_b5);
}

/* set function: joy_b5 */
static void toluaI_set_joy_b5(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_b5 = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_b6 */
static void toluaI_get_joy_b6(void)
{
  tolua_pushnumber((double)joy_b6);
}

/* set function: joy_b6 */
static void toluaI_set_joy_b6(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_b6 = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_hat */
static void toluaI_get_joy_hat(void)
{
  tolua_pushnumber((double)joy_hat);
}

/* set function: joy_hat */
static void toluaI_set_joy_hat(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_hat = ((int)  tolua_getnumber(1,0));
}

/* get function: joy_throttle */
static void toluaI_get_joy_throttle(void)
{
  tolua_pushnumber((double)joy_throttle);
}

/* set function: joy_throttle */
static void toluaI_set_joy_throttle(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy_throttle = ((int)  tolua_getnumber(1,0));
}

/* get function: joy2_x */
static void toluaI_get_joy2_x(void)
{
  tolua_pushnumber((double)joy2_x);
}

/* set function: joy2_x */
static void toluaI_set_joy2_x(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy2_x = ((int)  tolua_getnumber(1,0));
}

/* get function: joy2_y */
static void toluaI_get_joy2_y(void)
{
  tolua_pushnumber((double)joy2_y);
}

/* set function: joy2_y */
static void toluaI_set_joy2_y(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy2_y = ((int)  tolua_getnumber(1,0));
}

/* get function: joy2_left */
static void toluaI_get_joy2_left(void)
{
  tolua_pushnumber((double)joy2_left);
}

/* set function: joy2_left */
static void toluaI_set_joy2_left(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy2_left = ((int)  tolua_getnumber(1,0));
}

/* get function: joy2_right */
static void toluaI_get_joy2_right(void)
{
  tolua_pushnumber((double)joy2_right);
}

/* set function: joy2_right */
static void toluaI_set_joy2_right(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy2_right = ((int)  tolua_getnumber(1,0));
}

/* get function: joy2_up */
static void toluaI_get_joy2_up(void)
{
  tolua_pushnumber((double)joy2_up);
}

/* set function: joy2_up */
static void toluaI_set_joy2_up(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy2_up = ((int)  tolua_getnumber(1,0));
}

/* get function: joy2_down */
static void toluaI_get_joy2_down(void)
{
  tolua_pushnumber((double)joy2_down);
}

/* set function: joy2_down */
static void toluaI_set_joy2_down(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy2_down = ((int)  tolua_getnumber(1,0));
}

/* get function: joy2_b1 */
static void toluaI_get_joy2_b1(void)
{
  tolua_pushnumber((double)joy2_b1);
}

/* set function: joy2_b1 */
static void toluaI_set_joy2_b1(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy2_b1 = ((int)  tolua_getnumber(1,0));
}

/* get function: joy2_b2 */
static void toluaI_get_joy2_b2(void)
{
  tolua_pushnumber((double)joy2_b2);
}

/* set function: joy2_b2 */
static void toluaI_set_joy2_b2(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  joy2_b2 = ((int)  tolua_getnumber(1,0));
}

/* function: initialise_joystick */
static void toluaI_initialise_joystick0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   int toluaI_ret = initialise_joystick();
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'initialise_joystick'.");
}

/* function: calibrate_joystick_tl */
static void toluaI_calibrate_joystick_tl0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   int toluaI_ret = calibrate_joystick_tl();
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'calibrate_joystick_tl'.");
}

/* function: calibrate_joystick_br */
static void toluaI_calibrate_joystick_br0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   int toluaI_ret = calibrate_joystick_br();
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'calibrate_joystick_br'.");
}

/* function: calibrate_joystick_throttle_min */
static void toluaI_calibrate_joystick_throttle_min0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   int toluaI_ret = calibrate_joystick_throttle_min();
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'calibrate_joystick_throttle_min'.");
}

/* function: calibrate_joystick_throttle_max */
static void toluaI_calibrate_joystick_throttle_max0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   int toluaI_ret = calibrate_joystick_throttle_max();
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'calibrate_joystick_throttle_max'.");
}

/* function: calibrate_joystick_hat */
static void toluaI_calibrate_joystick_hat0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int direction = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = calibrate_joystick_hat(direction);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'calibrate_joystick_hat'.");
}

/* function: poll_joystick */
static void toluaI_poll_joystick0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   poll_joystick();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'poll_joystick'.");
}

/* function: save_joystick_data */
static void toluaI_save_joystick_data0(void)
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
   int toluaI_ret = save_joystick_data(filename);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'save_joystick_data'.");
}

/* function: load_joystick_data */
static void toluaI_load_joystick_data0(void)
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
   int toluaI_ret = load_joystick_data(filename);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_joystick_data'.");
}

/* get function: screen */
static void toluaI_get_screen(void)
{
  tolua_pushusertype((void*)screen,tolua_tag_BITMAP);
}

/* set function: screen */
static void toluaI_set_screen(void)
{
  if (!tolua_istype(1,tolua_tag_BITMAP,0))
   tolua_error("#vinvalid type in variable assignment.");
  screen = ((BITMAP*)  tolua_getusertype(1,0));
}

/* function: set_color_conversion */
static void toluaI_set_color_conversion0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int mode = ((int)  tolua_getnumber(1,0));
  {
   set_color_conversion(mode);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_color_conversion'.");
}

/* function: set_color_conversion */
static void toluaI_set_colour_conversion0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int mode = ((int)  tolua_getnumber(1,0));
  {
   set_color_conversion(mode);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_colour_conversion'.");
}

/* function: create_bitmap */
static void toluaI_create_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int width = ((int)  tolua_getnumber(1,0));
  int height = ((int)  tolua_getnumber(2,0));
  {
   BITMAP* toluaI_ret = create_bitmap(width,height);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'create_bitmap'.");
}

/* function: create_bitmap_ex */
static void toluaI_create_bitmap_ex0(void)
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
  int color_depth = ((int)  tolua_getnumber(1,0));
  int width = ((int)  tolua_getnumber(2,0));
  int height = ((int)  tolua_getnumber(3,0));
  {
   BITMAP* toluaI_ret = create_bitmap_ex(color_depth,width,height);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'create_bitmap_ex'.");
}

/* function: create_sub_bitmap */
static void toluaI_create_sub_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  BITMAP* parent = ((BITMAP*)  tolua_getusertype(1,0));
  int x = ((int)  tolua_getnumber(2,0));
  int y = ((int)  tolua_getnumber(3,0));
  int width = ((int)  tolua_getnumber(4,0));
  int height = ((int)  tolua_getnumber(5,0));
  {
   BITMAP* toluaI_ret = create_sub_bitmap(parent,x,y,width,height);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'create_sub_bitmap'.");
}

/* function: destroy_bitmap */
static void toluaI_destroy_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bitmap = ((BITMAP*)  tolua_getusertype(1,0));
  {
   destroy_bitmap(bitmap);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'destroy_bitmap'.");
}

/* function: bitmap_color_depth */
static void toluaI_bitmap_color_depth0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = bitmap_color_depth(bmp);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'bitmap_color_depth'.");
}

/* function: bitmap_mask_color */
static void toluaI_bitmap_mask_color0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = bitmap_mask_color(bmp);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'bitmap_mask_color'.");
}

/* function: is_same_bitmap */
static void toluaI_is_same_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  BITMAP* bmp1 = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* bmp2 = ((BITMAP*)  tolua_getusertype(2,0));
  {
   int toluaI_ret = is_same_bitmap(bmp1,bmp2);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'is_same_bitmap'.");
}

/* function: is_linear_bitmap */
static void toluaI_is_linear_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = is_linear_bitmap(bmp);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'is_linear_bitmap'.");
}

/* function: is_planar_bitmap */
static void toluaI_is_planar_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = is_planar_bitmap(bmp);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'is_planar_bitmap'.");
}

/* function: is_memory_bitmap */
static void toluaI_is_memory_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = is_memory_bitmap(bmp);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'is_memory_bitmap'.");
}

/* function: is_screen_bitmap */
static void toluaI_is_screen_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = is_screen_bitmap(bmp);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'is_screen_bitmap'.");
}

/* function: is_sub_bitmap */
static void toluaI_is_sub_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = is_sub_bitmap(bmp);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'is_sub_bitmap'.");
}

/* get function: _color_depth */
static void toluaI_get__color_depth(void)
{
  tolua_pushnumber((double)_color_depth);
}

/* set function: _color_depth */
static void toluaI_set__color_depth(void)
{
  if (!tolua_istype(1,tolua_tag_number,0))
   tolua_error("#vinvalid type in variable assignment.");
  _color_depth = ((int)  tolua_getnumber(1,0));
}

/* function: set_color */
static void toluaI_set_color0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_RGB,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int index = ((int)  tolua_getnumber(1,0));
  RGB* p = ((RGB*)  tolua_getusertype(2,0));
  {
   set_color(index,p);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_color'.");
}

/* function: set_color */
static void toluaI_set_colour0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_RGB,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int index = ((int)  tolua_getnumber(1,0));
  RGB* p = ((RGB*)  tolua_getusertype(2,0));
  {
   set_color(index,p);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_colour'.");
}

/* function: get_color */
static void toluaI_get_color0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_RGB,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int index = ((int)  tolua_getnumber(1,0));
  RGB* p = ((RGB*)  tolua_getusertype(2,0));
  {
   get_color(index,p);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_color'.");
}

/* function: fade_out_range */
static void toluaI_fade_out_range0(void)
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
  int speed = ((int)  tolua_getnumber(1,0));
  int from = ((int)  tolua_getnumber(2,0));
  int to = ((int)  tolua_getnumber(3,0));
  {
   fade_out_range(speed,from,to);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fade_out_range'.");
}

/* function: fade_out */
static void toluaI_fade_out0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int speed = ((int)  tolua_getnumber(1,0));
  {
   fade_out(speed);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fade_out'.");
}

/* function: unselect_pallete */
static void toluaI_unselect_pallete0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   unselect_pallete();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'unselect_pallete'.");
}

/* function: hsv_to_rgb */
static void toluaI_hsv_to_rgb0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  float h = ((float)  tolua_getnumber(1,0));
  float s = ((float)  tolua_getnumber(2,0));
  float v = ((float)  tolua_getnumber(3,0));
  int r = ((int)  tolua_getnumber(4,0));
  int g = ((int)  tolua_getnumber(5,0));
  int b = ((int)  tolua_getnumber(6,0));
  {
   hsv_to_rgb(h,s,v,&r,&g,&b);
   tolua_pushnumber((double)r);
   tolua_pushnumber((double)g);
   tolua_pushnumber((double)b);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'hsv_to_rgb'.");
}

/* function: rgb_to_hsv */
static void toluaI_rgb_to_hsv0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  int r = ((int)  tolua_getnumber(1,0));
  int g = ((int)  tolua_getnumber(2,0));
  int b = ((int)  tolua_getnumber(3,0));
  float h = ((float)  tolua_getnumber(4,0));
  float s = ((float)  tolua_getnumber(5,0));
  float v = ((float)  tolua_getnumber(6,0));
  {
   rgb_to_hsv(r,g,b,&h,&s,&v);
   tolua_pushnumber((double)h);
   tolua_pushnumber((double)s);
   tolua_pushnumber((double)v);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'rgb_to_hsv'.");
}

/* function: makecol */
static void toluaI_makecol0(void)
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
  int r = ((int)  tolua_getnumber(1,0));
  int g = ((int)  tolua_getnumber(2,0));
  int b = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = makecol(r,g,b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'makecol'.");
}

/* function: makecol8 */
static void toluaI_makecol80(void)
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
  int r = ((int)  tolua_getnumber(1,0));
  int g = ((int)  tolua_getnumber(2,0));
  int b = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = makecol8(r,g,b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'makecol8'.");
}

/* function: makecol15 */
static void toluaI_makecol150(void)
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
  int r = ((int)  tolua_getnumber(1,0));
  int g = ((int)  tolua_getnumber(2,0));
  int b = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = makecol15(r,g,b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'makecol15'.");
}

/* function: makecol16 */
static void toluaI_makecol160(void)
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
  int r = ((int)  tolua_getnumber(1,0));
  int g = ((int)  tolua_getnumber(2,0));
  int b = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = makecol16(r,g,b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'makecol16'.");
}

/* function: makecol24 */
static void toluaI_makecol240(void)
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
  int r = ((int)  tolua_getnumber(1,0));
  int g = ((int)  tolua_getnumber(2,0));
  int b = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = makecol24(r,g,b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'makecol24'.");
}

/* function: makecol32 */
static void toluaI_makecol320(void)
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
  int r = ((int)  tolua_getnumber(1,0));
  int g = ((int)  tolua_getnumber(2,0));
  int b = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = makecol32(r,g,b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'makecol32'.");
}

/* function: makecol_depth */
static void toluaI_makecol_depth0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  int color_depth = ((int)  tolua_getnumber(1,0));
  int r = ((int)  tolua_getnumber(2,0));
  int g = ((int)  tolua_getnumber(3,0));
  int b = ((int)  tolua_getnumber(4,0));
  {
   int toluaI_ret = makecol_depth(color_depth,r,g,b);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'makecol_depth'.");
}

/* function: getr */
static void toluaI_getr0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getr(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getr'.");
}

/* function: getg */
static void toluaI_getg0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getg(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getg'.");
}

/* function: getb */
static void toluaI_getb0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getb(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getb'.");
}

/* function: getr8 */
static void toluaI_getr80(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getr8(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getr8'.");
}

/* function: getg8 */
static void toluaI_getg80(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getg8(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getg8'.");
}

/* function: getb8 */
static void toluaI_getb80(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getb8(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getb8'.");
}

/* function: getr15 */
static void toluaI_getr150(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getr15(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getr15'.");
}

/* function: getg15 */
static void toluaI_getg150(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getg15(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getg15'.");
}

/* function: getb15 */
static void toluaI_getb150(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getb15(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getb15'.");
}

/* function: getr16 */
static void toluaI_getr160(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getr16(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getr16'.");
}

/* function: getg16 */
static void toluaI_getg160(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getg16(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getg16'.");
}

/* function: getb16 */
static void toluaI_getb160(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getb16(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getb16'.");
}

/* function: getr24 */
static void toluaI_getr240(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getr24(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getr24'.");
}

/* function: getg24 */
static void toluaI_getg240(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getg24(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getg24'.");
}

/* function: getb24 */
static void toluaI_getb240(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getb24(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getb24'.");
}

/* function: getr32 */
static void toluaI_getr320(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getr32(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getr32'.");
}

/* function: getg32 */
static void toluaI_getg320(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getg32(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getg32'.");
}

/* function: getb32 */
static void toluaI_getb320(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int c = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = getb32(c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getb32'.");
}

/* function: getr_depth */
static void toluaI_getr_depth0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int color_depth = ((int)  tolua_getnumber(1,0));
  int c = ((int)  tolua_getnumber(2,0));
  {
   int toluaI_ret = getr_depth(color_depth,c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getr_depth'.");
}

/* function: getg_depth */
static void toluaI_getg_depth0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int color_depth = ((int)  tolua_getnumber(1,0));
  int c = ((int)  tolua_getnumber(2,0));
  {
   int toluaI_ret = getg_depth(color_depth,c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getg_depth'.");
}

/* function: getb_depth */
static void toluaI_getb_depth0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int color_depth = ((int)  tolua_getnumber(1,0));
  int c = ((int)  tolua_getnumber(2,0));
  {
   int toluaI_ret = getb_depth(color_depth,c);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getb_depth'.");
}

/* function: set_clip */
static void toluaI_set_clip0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  BITMAP* bitmap = ((BITMAP*)  tolua_getusertype(1,0));
  int x1 = ((int)  tolua_getnumber(2,0));
  int y1 = ((int)  tolua_getnumber(3,0));
  int x2 = ((int)  tolua_getnumber(4,0));
  int y2 = ((int)  tolua_getnumber(5,0));
  {
   set_clip(bitmap,x1,y1,x2,y2);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_clip'.");
}

/* function: drawing_mode */
static void toluaI_drawing_mode0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  int mode = ((int)  tolua_getnumber(1,0));
  BITMAP* pattern = ((BITMAP*)  tolua_getusertype(2,0));
  int x_anchor = ((int)  tolua_getnumber(3,0));
  int y_anchor = ((int)  tolua_getnumber(4,0));
  {
   drawing_mode(mode,pattern,x_anchor,y_anchor);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'drawing_mode'.");
}

/* function: xor_mode */
static void toluaI_xor_mode0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int xor = ((int)  tolua_getnumber(1,0));
  {
   xor_mode(xor);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'xor_mode'.");
}

/* function: solid_mode */
static void toluaI_solid_mode0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   solid_mode();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'solid_mode'.");
}

/* function: getpixel */
static void toluaI_getpixel0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x = ((int)  tolua_getnumber(2,0));
  int y = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = getpixel(bmp,x,y);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'getpixel'.");
}

/* function: putpixel */
static void toluaI_putpixel0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x = ((int)  tolua_getnumber(2,0));
  int y = ((int)  tolua_getnumber(3,0));
  int color = ((int)  tolua_getnumber(4,0));
  {
   putpixel(bmp,x,y,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'putpixel'.");
}

/* function: vline */
static void toluaI_vline0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x = ((int)  tolua_getnumber(2,0));
  int y1 = ((int)  tolua_getnumber(3,0));
  int y2 = ((int)  tolua_getnumber(4,0));
  int color = ((int)  tolua_getnumber(5,0));
  {
   vline(bmp,x,y1,y2,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'vline'.");
}

/* function: hline */
static void toluaI_hline0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x1 = ((int)  tolua_getnumber(2,0));
  int y = ((int)  tolua_getnumber(3,0));
  int x2 = ((int)  tolua_getnumber(4,0));
  int color = ((int)  tolua_getnumber(5,0));
  {
   hline(bmp,x1,y,x2,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'hline'.");
}

/* function: line */
static void toluaI_line0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x1 = ((int)  tolua_getnumber(2,0));
  int y1 = ((int)  tolua_getnumber(3,0));
  int x2 = ((int)  tolua_getnumber(4,0));
  int y2 = ((int)  tolua_getnumber(5,0));
  int color = ((int)  tolua_getnumber(6,0));
  {
   line(bmp,x1,y1,x2,y2,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'line'.");
}

/* function: rectfill */
static void toluaI_rectfill0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x1 = ((int)  tolua_getnumber(2,0));
  int y1 = ((int)  tolua_getnumber(3,0));
  int x2 = ((int)  tolua_getnumber(4,0));
  int y2 = ((int)  tolua_getnumber(5,0));
  int color = ((int)  tolua_getnumber(6,0));
  {
   rectfill(bmp,x1,y1,x2,y2,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'rectfill'.");
}

/* function: draw_sprite */
static void toluaI_draw_sprite0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  {
   draw_sprite(bmp,sprite,x,y);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'draw_sprite'.");
}

/* function: draw_sprite_v_flip */
static void toluaI_draw_sprite_v_flip0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  {
   draw_sprite_v_flip(bmp,sprite,x,y);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'draw_sprite_v_flip'.");
}

/* function: draw_sprite_h_flip */
static void toluaI_draw_sprite_h_flip0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  {
   draw_sprite_h_flip(bmp,sprite,x,y);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'draw_sprite_h_flip'.");
}

/* function: draw_sprite_vh_flip */
static void toluaI_draw_sprite_vh_flip0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  {
   draw_sprite_vh_flip(bmp,sprite,x,y);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'draw_sprite_vh_flip'.");
}

/* function: draw_trans_sprite */
static void toluaI_draw_trans_sprite0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  {
   draw_trans_sprite(bmp,sprite,x,y);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'draw_trans_sprite'.");
}

/* function: draw_lit_sprite */
static void toluaI_draw_lit_sprite0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  int color = ((int)  tolua_getnumber(5,0));
  {
   draw_lit_sprite(bmp,sprite,x,y,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'draw_lit_sprite'.");
}

/* function: draw_character */
static void toluaI_draw_character0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  int color = ((int)  tolua_getnumber(5,0));
  {
   draw_character(bmp,sprite,x,y,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'draw_character'.");
}

/* function: clear_to_color */
static void toluaI_clear_to_color0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  BITMAP* bitmap = ((BITMAP*)  tolua_getusertype(1,0));
  int color = ((int)  tolua_getnumber(2,0));
  {
   clear_to_color(bitmap,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'clear_to_color'.");
}

/* function: triangle */
static void toluaI_triangle0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_istype(7,tolua_tag_number,0) ||
     !tolua_istype(8,tolua_tag_number,0) ||
     !tolua_isnoobj(9)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x1 = ((int)  tolua_getnumber(2,0));
  int y1 = ((int)  tolua_getnumber(3,0));
  int x2 = ((int)  tolua_getnumber(4,0));
  int y2 = ((int)  tolua_getnumber(5,0));
  int x3 = ((int)  tolua_getnumber(6,0));
  int y3 = ((int)  tolua_getnumber(7,0));
  int color = ((int)  tolua_getnumber(8,0));
  {
   triangle(bmp,x1,y1,x2,y2,x3,y3,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'triangle'.");
}

/* function: polygon */
static void toluaI_polygon0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int vertices = ((int)  tolua_getnumber(2,0));
  int points = ((int)  tolua_getnumber(3,0));
  int color = ((int)  tolua_getnumber(4,0));
  {
   polygon(bmp,vertices,&points,color);
   tolua_pushnumber((double)points);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'polygon'.");
}

/* function: rect */
static void toluaI_rect0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x1 = ((int)  tolua_getnumber(2,0));
  int y1 = ((int)  tolua_getnumber(3,0));
  int x2 = ((int)  tolua_getnumber(4,0));
  int y2 = ((int)  tolua_getnumber(5,0));
  int color = ((int)  tolua_getnumber(6,0));
  {
   rect(bmp,x1,y1,x2,y2,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'rect'.");
}

/* function: circle */
static void toluaI_circle0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x = ((int)  tolua_getnumber(2,0));
  int y = ((int)  tolua_getnumber(3,0));
  int radius = ((int)  tolua_getnumber(4,0));
  int color = ((int)  tolua_getnumber(5,0));
  {
   circle(bmp,x,y,radius,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'circle'.");
}

/* function: circlefill */
static void toluaI_circlefill0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x = ((int)  tolua_getnumber(2,0));
  int y = ((int)  tolua_getnumber(3,0));
  int radius = ((int)  tolua_getnumber(4,0));
  int color = ((int)  tolua_getnumber(5,0));
  {
   circlefill(bmp,x,y,radius,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'circlefill'.");
}

/* function: ellipse */
static void toluaI_ellipse0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x = ((int)  tolua_getnumber(2,0));
  int y = ((int)  tolua_getnumber(3,0));
  int rx = ((int)  tolua_getnumber(4,0));
  int ry = ((int)  tolua_getnumber(5,0));
  int color = ((int)  tolua_getnumber(6,0));
  {
   ellipse(bmp,x,y,rx,ry,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'ellipse'.");
}

/* function: ellipsefill */
static void toluaI_ellipsefill0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int cx = ((int)  tolua_getnumber(2,0));
  int cy = ((int)  tolua_getnumber(3,0));
  int rx = ((int)  tolua_getnumber(4,0));
  int ry = ((int)  tolua_getnumber(5,0));
  int color = ((int)  tolua_getnumber(6,0));
  {
   ellipsefill(bmp,cx,cy,rx,ry,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'ellipsefill'.");
}

/* function: calc_spline */
static void toluaI_calc_spline0(void)
{
 if (
     !tolua_istype(1,tolua_tag_table,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  int points[8];
  int npts = ((int)  tolua_getnumber(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  {
   if (!tolua_arrayistype(1,tolua_tag_number,8,0))
    goto error;
   else
   {
    int i;
    lua_Object lo = lua_getparam(1);
    for(i=0; i<8;i++)
    points[i] = ((int)  tolua_getfieldnumber(lo,i+1,0));
   }
  }
  {
   calc_spline(points,npts,&x,&y);
   tolua_pushnumber((double)x);
   tolua_pushnumber((double)y);
  }
  {
   int i;
   lua_Object lo = lua_getparam(1);
   for(i=0; i<8;i++)
   tolua_pushfieldnumber(lo,i+1,(double) points[i]);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'calc_spline'.");
}

/* function: spline */
static void toluaI_spline0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_table,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int points[8];
  int color = ((int)  tolua_getnumber(3,0));
  {
   if (!tolua_arrayistype(2,tolua_tag_number,8,0))
    goto error;
   else
   {
    int i;
    lua_Object lo = lua_getparam(2);
    for(i=0; i<8;i++)
    points[i] = ((int)  tolua_getfieldnumber(lo,i+1,0));
   }
  }
  {
   spline(bmp,points,color);
  }
  {
   int i;
   lua_Object lo = lua_getparam(2);
   for(i=0; i<8;i++)
   tolua_pushfieldnumber(lo,i+1,(double) points[i]);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'spline'.");
}

/* function: floodfill */
static void toluaI_floodfill0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_isnoobj(5)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  int x = ((int)  tolua_getnumber(2,0));
  int y = ((int)  tolua_getnumber(3,0));
  int color = ((int)  tolua_getnumber(4,0));
  {
   floodfill(bmp,x,y,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'floodfill'.");
}

/* function: blit */
static void toluaI_blit0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_istype(7,tolua_tag_number,0) ||
     !tolua_istype(8,tolua_tag_number,0) ||
     !tolua_isnoobj(9)
 )
  goto error;
 else
 {
  BITMAP* source = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* dest = ((BITMAP*)  tolua_getusertype(2,0));
  int source_x = ((int)  tolua_getnumber(3,0));
  int source_y = ((int)  tolua_getnumber(4,0));
  int dest_x = ((int)  tolua_getnumber(5,0));
  int dest_y = ((int)  tolua_getnumber(6,0));
  int width = ((int)  tolua_getnumber(7,0));
  int height = ((int)  tolua_getnumber(8,0));
  {
   blit(source,dest,source_x,source_y,dest_x,dest_y,width,height);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'blit'.");
}

/* function: masked_blit */
static void toluaI_masked_blit0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_istype(7,tolua_tag_number,0) ||
     !tolua_istype(8,tolua_tag_number,0) ||
     !tolua_isnoobj(9)
 )
  goto error;
 else
 {
  BITMAP* source = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* dest = ((BITMAP*)  tolua_getusertype(2,0));
  int source_x = ((int)  tolua_getnumber(3,0));
  int source_y = ((int)  tolua_getnumber(4,0));
  int dest_x = ((int)  tolua_getnumber(5,