/*
** Lua binding: allegro
** Generated automatically by tolua 3.0b on Sun Jun 27 18:38:53 1999.
*/

/* Exported function: package initialization */
int tolua_allegro_open (void);

#include "tolua.h"

#include <allegro.h>
#include "allegrobindings.h"

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
 tolua_usertype("MIDI");
 tolua_usertype("RGB");
 tolua_usertype("SAMPLE");
 tolua_usertype("DATAFILE");
 tolua_usertype("BITMAP");
 tolua_usertype("PACKFILE");
 tolua_usertype("FONT");
 tolua_usertype("AUDIOSTREAM");
 tolua_settag("RGB",&tolua_tag_RGB);
 tolua_settag("SAMPLE",&tolua_tag_SAMPLE);
 tolua_settag("PACKFILE",&tolua_tag_PACKFILE);
 tolua_settag("BITMAP",&tolua_tag_BITMAP);
 tolua_settag("AUDIOSTREAM",&tolua_tag_AUDIOSTREAM);
 tolua_settag("DATAFILE",&tolua_tag_DATAFILE);
 tolua_settag("MIDI",&tolua_tag_MIDI);
 tolua_settag("FONT",&tolua_tag_FONT);
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
  int dest_x = ((int)  tolua_getnumber(5,0));
  int dest_y = ((int)  tolua_getnumber(6,0));
  int width = ((int)  tolua_getnumber(7,0));
  int height = ((int)  tolua_getnumber(8,0));
  {
   masked_blit(source,dest,source_x,source_y,dest_x,dest_y,width,height);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'masked_blit'.");
}

/* function: stretch_blit */
static void toluaI_stretch_blit0(void)
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
     !tolua_istype(9,tolua_tag_number,0) ||
     !tolua_istype(10,tolua_tag_number,0) ||
     !tolua_isnoobj(11)
 )
  goto error;
 else
 {
  BITMAP* s = ((BITMAP*)  tolua_getusertype(1,0));
  BITMAP* d = ((BITMAP*)  tolua_getusertype(2,0));
  int s_x = ((int)  tolua_getnumber(3,0));
  int s_y = ((int)  tolua_getnumber(4,0));
  int s_w = ((int)  tolua_getnumber(5,0));
  int s_h = ((int)  tolua_getnumber(6,0));
  int d_x = ((int)  tolua_getnumber(7,0));
  int d_y = ((int)  tolua_getnumber(8,0));
  int d_w = ((int)  tolua_getnumber(9,0));
  int d_h = ((int)  tolua_getnumber(10,0));
  {
   stretch_blit(s,d,s_x,s_y,s_w,s_h,d_x,d_y,d_w,d_h);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'stretch_blit'.");
}

/* function: stretch_sprite */
static void toluaI_stretch_sprite0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
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
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  int w = ((int)  tolua_getnumber(5,0));
  int h = ((int)  tolua_getnumber(6,0));
  {
   stretch_sprite(bmp,sprite,x,y,w,h);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'stretch_sprite'.");
}

/* function: rotate_sprite */
static void toluaI_rotate_sprite0(void)
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
  fixed angle = ((fixed)  tolua_getnumber(5,0));
  {
   rotate_sprite(bmp,sprite,x,y,angle);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'rotate_sprite'.");
}

/* function: rotate_scaled_sprite */
static void toluaI_rotate_scaled_sprite0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
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
  BITMAP* sprite = ((BITMAP*)  tolua_getusertype(2,0));
  int x = ((int)  tolua_getnumber(3,0));
  int y = ((int)  tolua_getnumber(4,0));
  fixed angle = ((fixed)  tolua_getnumber(5,0));
  fixed scale = ((fixed)  tolua_getnumber(6,0));
  {
   rotate_scaled_sprite(bmp,sprite,x,y,angle,scale);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'rotate_scaled_sprite'.");
}

/* function: clear */
static void toluaI_clear0(void)
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
   clear(bitmap);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'clear'.");
}

/* get function: font */
static void toluaI_get_font(void)
{
  tolua_pushusertype((void*)font,tolua_tag_FONT);
}

/* set function: font */
static void toluaI_set_font(void)
{
  if (!tolua_istype(1,tolua_tag_FONT,0))
   tolua_error("#vinvalid type in variable assignment.");
  font = ((FONT*)  tolua_getusertype(1,0));
}

/* function: text_mode */
static void toluaI_text_mode0(void)
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
   text_mode(mode);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'text_mode'.");
}

/* function: textout */
static void toluaI_textout0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_FONT,0) ||
     !tolua_istype(3,tolua_tag_string,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  FONT* f = ((FONT*)  tolua_getusertype(2,0));
  unsigned char* str = ((unsigned char*)  tolua_getstring(3,0));
  int x = ((int)  tolua_getnumber(4,0));
  int y = ((int)  tolua_getnumber(5,0));
  int color = ((int)  tolua_getnumber(6,0));
  {
   textout(bmp,f,str,x,y,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'textout'.");
}

/* function: textout_centre */
static void toluaI_textout_centre0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_FONT,0) ||
     !tolua_istype(3,tolua_tag_string,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_isnoobj(7)
 )
  goto error;
 else
 {
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(1,0));
  FONT* f = ((FONT*)  tolua_getusertype(2,0));
  unsigned char* str = ((unsigned char*)  tolua_getstring(3,0));
  int x = ((int)  tolua_getnumber(4,0));
  int y = ((int)  tolua_getnumber(5,0));
  int color = ((int)  tolua_getnumber(6,0));
  {
   textout_centre(bmp,f,str,x,y,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'textout_centre'.");
}

/* function: textout_justify */
static void toluaI_textout_justify0(void)
{
 if (
     !tolua_istype(1,tolua_tag_BITMAP,0) ||
     !tolua_istype(2,tolua_tag_FONT,0) ||
     !tolua_istype(3,tolua_tag_string,0) ||
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
  FONT* f = ((FONT*)  tolua_getusertype(2,0));
  unsigned char* str = ((unsigned char*)  tolua_getstring(3,0));
  int x1 = ((int)  tolua_getnumber(4,0));
  int x2 = ((int)  tolua_getnumber(5,0));
  int y = ((int)  tolua_getnumber(6,0));
  int diff = ((int)  tolua_getnumber(7,0));
  int color = ((int)  tolua_getnumber(8,0));
  {
   textout_justify(bmp,f,str,x1,x2,y,diff,color);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'textout_justify'.");
}

/* function: text_length */
static void toluaI_text_length0(void)
{
 if (
     !tolua_istype(1,tolua_tag_FONT,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  FONT* f = ((FONT*)  tolua_getusertype(1,0));
  unsigned char* str = ((unsigned char*)  tolua_getstring(2,0));
  {
   int toluaI_ret = text_length(f,str);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'text_length'.");
}

/* function: text_height */
static void toluaI_text_height0(void)
{
 if (
     !tolua_istype(1,tolua_tag_FONT,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  FONT* f = ((FONT*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = text_height(f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'text_height'.");
}

/* function: destroy_font */
static void toluaI_destroy_font0(void)
{
 if (
     !tolua_istype(1,tolua_tag_FONT,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  FONT* f = ((FONT*)  tolua_getusertype(1,0));
  {
   destroy_font(f);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'destroy_font'.");
}

/* function: reserve_voices */
static void toluaI_reserve_voices0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int digi_voices = ((int)  tolua_getnumber(1,0));
  int midi_voices = ((int)  tolua_getnumber(2,0));
  {
   reserve_voices(digi_voices,midi_voices);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'reserve_voices'.");
}

/* function: set_volume */
static void toluaI_set_volume0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int digi_volume = ((int)  tolua_getnumber(1,0));
  int midi_volume = ((int)  tolua_getnumber(2,0));
  {
   set_volume(digi_volume,midi_volume);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'set_volume'.");
}

/* function: load_sample */
static void toluaI_load_sample0(void)
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
   SAMPLE* toluaI_ret = load_sample(filename);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_SAMPLE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_sample'.");
}

/* function: load_wav */
static void toluaI_load_wav0(void)
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
   SAMPLE* toluaI_ret = load_wav(filename);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_SAMPLE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_wav'.");
}

/* function: load_voc */
static void toluaI_load_voc0(void)
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
   SAMPLE* toluaI_ret = load_voc(filename);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_SAMPLE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_voc'.");
}

/* function: destroy_sample */
static void toluaI_destroy_sample0(void)
{
 if (
     !tolua_istype(1,tolua_tag_SAMPLE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  SAMPLE* spl = ((SAMPLE*)  tolua_getusertype(1,0));
  {
   destroy_sample(spl);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'destroy_sample'.");
}

/* function: play_sample */
static void toluaI_play_sample0(void)
{
 if (
     !tolua_istype(1,tolua_tag_SAMPLE,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  SAMPLE* spl = ((SAMPLE*)  tolua_getusertype(1,0));
  int vol = ((int)  tolua_getnumber(2,0));
  int pan = ((int)  tolua_getnumber(3,0));
  int freq = ((int)  tolua_getnumber(4,0));
  int loop = ((int)  tolua_getnumber(5,0));
  {
   int toluaI_ret = play_sample(spl,vol,pan,freq,loop);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'play_sample'.");
}

/* function: stop_sample */
static void toluaI_stop_sample0(void)
{
 if (
     !tolua_istype(1,tolua_tag_SAMPLE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  SAMPLE* spl = ((SAMPLE*)  tolua_getusertype(1,0));
  {
   stop_sample(spl);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'stop_sample'.");
}

/* function: adjust_sample */
static void toluaI_adjust_sample0(void)
{
 if (
     !tolua_istype(1,tolua_tag_SAMPLE,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  SAMPLE* spl = ((SAMPLE*)  tolua_getusertype(1,0));
  int vol = ((int)  tolua_getnumber(2,0));
  int pan = ((int)  tolua_getnumber(3,0));
  int freq = ((int)  tolua_getnumber(4,0));
  int loop = ((int)  tolua_getnumber(5,0));
  {
   adjust_sample(spl,vol,pan,freq,loop);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'adjust_sample'.");
}

/* function: allocate_voice */
static void toluaI_allocate_voice0(void)
{
 if (
     !tolua_istype(1,tolua_tag_SAMPLE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  SAMPLE* spl = ((SAMPLE*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = allocate_voice(spl);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'allocate_voice'.");
}

/* function: deallocate_voice */
static void toluaI_deallocate_voice0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   deallocate_voice(voice);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'deallocate_voice'.");
}

/* function: reallocate_voice */
static void toluaI_reallocate_voice0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_SAMPLE,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  SAMPLE* spl = ((SAMPLE*)  tolua_getusertype(2,0));
  {
   reallocate_voice(voice,spl);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'reallocate_voice'.");
}

/* function: release_voice */
static void toluaI_release_voice0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   release_voice(voice);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'release_voice'.");
}

/* function: voice_start */
static void toluaI_voice_start0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   voice_start(voice);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_start'.");
}

/* function: voice_stop */
static void toluaI_voice_stop0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   voice_stop(voice);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_stop'.");
}

/* function: voice_set_priority */
static void toluaI_voice_set_priority0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  int priority = ((int)  tolua_getnumber(2,0));
  {
   voice_set_priority(voice,priority);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_priority'.");
}

/* function: voice_check */
static void toluaI_voice_check0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   SAMPLE* toluaI_ret = voice_check(voice);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_SAMPLE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_check'.");
}

/* function: voice_set_playmode */
static void toluaI_voice_set_playmode0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  int playmode = ((int)  tolua_getnumber(2,0));
  {
   voice_set_playmode(voice,playmode);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_playmode'.");
}

/* function: voice_get_position */
static void toluaI_voice_get_position0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = voice_get_position(voice);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_get_position'.");
}

/* function: voice_set_position */
static void toluaI_voice_set_position0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  int position = ((int)  tolua_getnumber(2,0));
  {
   voice_set_position(voice,position);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_position'.");
}

/* function: voice_get_volume */
static void toluaI_voice_get_volume0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = voice_get_volume(voice);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_get_volume'.");
}

/* function: voice_set_volume */
static void toluaI_voice_set_volume0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  int volume = ((int)  tolua_getnumber(2,0));
  {
   voice_set_volume(voice,volume);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_volume'.");
}

/* function: voice_ramp_volume */
static void toluaI_voice_ramp_volume0(void)
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
  int voice = ((int)  tolua_getnumber(1,0));
  int time = ((int)  tolua_getnumber(2,0));
  int endvol = ((int)  tolua_getnumber(3,0));
  {
   voice_ramp_volume(voice,time,endvol);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_ramp_volume'.");
}

/* function: voice_stop_volumeramp */
static void toluaI_voice_stop_volumeramp0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   voice_stop_volumeramp(voice);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_stop_volumeramp'.");
}

/* function: voice_get_frequency */
static void toluaI_voice_get_frequency0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = voice_get_frequency(voice);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_get_frequency'.");
}

/* function: voice_set_frequency */
static void toluaI_voice_set_frequency0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  int frequency = ((int)  tolua_getnumber(2,0));
  {
   voice_set_frequency(voice,frequency);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_frequency'.");
}

/* function: voice_sweep_frequency */
static void toluaI_voice_sweep_frequency0(void)
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
  int voice = ((int)  tolua_getnumber(1,0));
  int time = ((int)  tolua_getnumber(2,0));
  int endfreq = ((int)  tolua_getnumber(3,0));
  {
   voice_sweep_frequency(voice,time,endfreq);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_sweep_frequency'.");
}

/* function: voice_stop_frequency_sweep */
static void toluaI_voice_stop_frequency_sweep0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   voice_stop_frequency_sweep(voice);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_stop_frequency_sweep'.");
}

/* function: voice_get_pan */
static void toluaI_voice_get_pan0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = voice_get_pan(voice);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_get_pan'.");
}

/* function: voice_set_pan */
static void toluaI_voice_set_pan0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  int pan = ((int)  tolua_getnumber(2,0));
  {
   voice_set_pan(voice,pan);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_pan'.");
}

/* function: voice_sweep_pan */
static void toluaI_voice_sweep_pan0(void)
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
  int voice = ((int)  tolua_getnumber(1,0));
  int time = ((int)  tolua_getnumber(2,0));
  int endpan = ((int)  tolua_getnumber(3,0));
  {
   voice_sweep_pan(voice,time,endpan);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_sweep_pan'.");
}

/* function: voice_stop_pan_sweep */
static void toluaI_voice_stop_pan_sweep0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int voice = ((int)  tolua_getnumber(1,0));
  {
   voice_stop_pan_sweep(voice);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_stop_pan_sweep'.");
}

/* function: voice_set_echo */
static void toluaI_voice_set_echo0(void)
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
  int voice = ((int)  tolua_getnumber(1,0));
  int strength = ((int)  tolua_getnumber(2,0));
  int delay = ((int)  tolua_getnumber(3,0));
  {
   voice_set_echo(voice,strength,delay);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_echo'.");
}

/* function: voice_set_tremolo */
static void toluaI_voice_set_tremolo0(void)
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
  int voice = ((int)  tolua_getnumber(1,0));
  int rate = ((int)  tolua_getnumber(2,0));
  int depth = ((int)  tolua_getnumber(3,0));
  {
   voice_set_tremolo(voice,rate,depth);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_tremolo'.");
}

/* function: voice_set_vibrato */
static void toluaI_voice_set_vibrato0(void)
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
  int voice = ((int)  tolua_getnumber(1,0));
  int rate = ((int)  tolua_getnumber(2,0));
  int depth = ((int)  tolua_getnumber(3,0));
  {
   voice_set_vibrato(voice,rate,depth);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'voice_set_vibrato'.");
}

/* function: load_midi */
static void toluaI_load_midi0(void)
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
   MIDI* toluaI_ret = load_midi(filename);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_MIDI);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_midi'.");
}

/* function: destroy_midi */
static void toluaI_destroy_midi0(void)
{
 if (
     !tolua_istype(1,tolua_tag_MIDI,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  MIDI* midi = ((MIDI*)  tolua_getusertype(1,0));
  {
   destroy_midi(midi);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'destroy_midi'.");
}

/* function: play_midi */
static void toluaI_play_midi0(void)
{
 if (
     !tolua_istype(1,tolua_tag_MIDI,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  MIDI* midi = ((MIDI*)  tolua_getusertype(1,0));
  int loop = ((int)  tolua_getnumber(2,0));
  {
   int toluaI_ret = play_midi(midi,loop);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'play_midi'.");
}

/* function: play_looped_midi */
static void toluaI_play_looped_midi0(void)
{
 if (
     !tolua_istype(1,tolua_tag_MIDI,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  MIDI* midi = ((MIDI*)  tolua_getusertype(1,0));
  int loop_start = ((int)  tolua_getnumber(2,0));
  int loop_end = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = play_looped_midi(midi,loop_start,loop_end);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'play_looped_midi'.");
}

/* function: stop_midi */
static void toluaI_stop_midi0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   stop_midi();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'stop_midi'.");
}

/* function: midi_pause */
static void toluaI_midi_pause0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   midi_pause();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'midi_pause'.");
}

/* function: midi_resume */
static void toluaI_midi_resume0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   midi_resume();
  }
 }
 return;
error:
 tolua_error("#ferror in function 'midi_resume'.");
}

/* function: midi_seek */
static void toluaI_midi_seek0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int target = ((int)  tolua_getnumber(1,0));
  {
   int toluaI_ret = midi_seek(target);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'midi_seek'.");
}

/* function: midi_out */
static void toluaI_midi_out0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  unsigned char* data = ((unsigned char*)  tolua_getstring(1,0));
  int length = ((int)  tolua_getnumber(2,0));
  {
   midi_out(data,length);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'midi_out'.");
}

/* function: load_midi_patches */
static void toluaI_load_midi_patches0(void)
{
 if (
     !tolua_isnoobj(1)
 )
  goto error;
 else
 {
  {
   int toluaI_ret = load_midi_patches();
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_midi_patches'.");
}

/* function: play_audio_stream */
static void toluaI_play_audio_stream0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_istype(4,tolua_tag_number,0) ||
     !tolua_istype(5,tolua_tag_number,0) ||
     !tolua_isnoobj(6)
 )
  goto error;
 else
 {
  int len = ((int)  tolua_getnumber(1,0));
  int bits = ((int)  tolua_getnumber(2,0));
  int freq = ((int)  tolua_getnumber(3,0));
  int vol = ((int)  tolua_getnumber(4,0));
  int pan = ((int)  tolua_getnumber(5,0));
  {
   AUDIOSTREAM* toluaI_ret = play_audio_stream(len,bits,freq,vol,pan);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_AUDIOSTREAM);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'play_audio_stream'.");
}

/* function: stop_audio_stream */
static void toluaI_stop_audio_stream0(void)
{
 if (
     !tolua_istype(1,tolua_tag_AUDIOSTREAM,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  AUDIOSTREAM* stream = ((AUDIOSTREAM*)  tolua_getusertype(1,0));
  {
   stop_audio_stream(stream);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'stop_audio_stream'.");
}

/* function: get_audio_stream_buffer */
static void toluaI_get_audio_stream_buffer0(void)
{
 if (
     !tolua_istype(1,tolua_tag_AUDIOSTREAM,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  AUDIOSTREAM* stream = ((AUDIOSTREAM*)  tolua_getusertype(1,0));
  {
   void* toluaI_ret = get_audio_stream_buffer(stream);
   tolua_pushuserdata(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_audio_stream_buffer'.");
}

/* function: free_audio_stream_buffer */
static void toluaI_free_audio_stream_buffer0(void)
{
 if (
     !tolua_istype(1,tolua_tag_AUDIOSTREAM,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  AUDIOSTREAM* stream = ((AUDIOSTREAM*)  tolua_getusertype(1,0));
  {
   free_audio_stream_buffer(stream);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'free_audio_stream_buffer'.");
}

/* function: get_filename */
static void toluaI_get_filename0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  char* path = ((char*)  tolua_getstring(1,0));
  {
   char* toluaI_ret = get_filename(path);
   tolua_pushstring(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_filename'.");
}

/* function: get_extension */
static void toluaI_get_extension0(void)
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
   char* toluaI_ret = get_extension(filename);
   tolua_pushstring(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_extension'.");
}

/* function: put_backslash */
static void toluaI_put_backslash0(void)
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
   put_backslash(filename);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'put_backslash'.");
}

/* function: file_exists */
static void toluaI_file_exists0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_number,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  int attrib = ((int)  tolua_getnumber(2,0));
  int aret = ((int)  tolua_getnumber(3,0));
  {
   int toluaI_ret = file_exists(filename,attrib,&aret);
   tolua_pushnumber((double)toluaI_ret);
   tolua_pushnumber((double)aret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'file_exists'.");
}

/* function: exists */
static void toluaI_exists0(void)
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
   int toluaI_ret = exists(filename);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'exists'.");
}

/* function: file_size */
static void toluaI_file_size0(void)
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
   long toluaI_ret = file_size(filename);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'file_size'.");
}

/* function: file_time */
static void toluaI_file_time0(void)
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
   long toluaI_ret = file_time(filename);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'file_time'.");
}

/* function: packfile_password */
static void toluaI_packfile_password0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  char* password = ((char*)  tolua_getstring(1,0));
  {
   packfile_password(password);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'packfile_password'.");
}

/* function: pack_fopen */
static void toluaI_pack_fopen0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  char* mode = ((char*)  tolua_getstring(2,0));
  {
   PACKFILE* toluaI_ret = pack_fopen(filename,mode);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_PACKFILE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fopen'.");
}

/* function: pack_fclose */
static void toluaI_pack_fclose0(void)
{
 if (
     !tolua_istype(1,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = pack_fclose(f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fclose'.");
}

/* function: pack_fseek */
static void toluaI_pack_fseek0(void)
{
 if (
     !tolua_istype(1,tolua_tag_PACKFILE,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(1,0));
  int offset = ((int)  tolua_getnumber(2,0));
  {
   int toluaI_ret = pack_fseek(f,offset);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fseek'.");
}

/* function: pack_fopen_chunk */
static void toluaI_pack_fopen_chunk0(void)
{
 if (
     !tolua_istype(1,tolua_tag_PACKFILE,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(1,0));
  int pack = ((int)  tolua_getnumber(2,0));
  {
   PACKFILE* toluaI_ret = pack_fopen_chunk(f,pack);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_PACKFILE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fopen_chunk'.");
}

/* function: pack_fclose_chunk */
static void toluaI_pack_fclose_chunk0(void)
{
 if (
     !tolua_istype(1,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(1,0));
  {
   PACKFILE* toluaI_ret = pack_fclose_chunk(f);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_PACKFILE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fclose_chunk'.");
}

/* function: pack_igetw */
static void toluaI_pack_igetw0(void)
{
 if (
     !tolua_istype(1,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = pack_igetw(f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_igetw'.");
}

/* function: pack_igetl */
static void toluaI_pack_igetl0(void)
{
 if (
     !tolua_istype(1,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(1,0));
  {
   long toluaI_ret = pack_igetl(f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_igetl'.");
}

/* function: pack_iputw */
static void toluaI_pack_iputw0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int w = ((int)  tolua_getnumber(1,0));
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(2,0));
  {
   int toluaI_ret = pack_iputw(w,f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_iputw'.");
}

/* function: pack_iputl */
static void toluaI_pack_iputl0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  long l = ((long)  tolua_getnumber(1,0));
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(2,0));
  {
   long toluaI_ret = pack_iputl(l,f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_iputl'.");
}

/* function: pack_mgetw */
static void toluaI_pack_mgetw0(void)
{
 if (
     !tolua_istype(1,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(1,0));
  {
   int toluaI_ret = pack_mgetw(f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_mgetw'.");
}

/* function: pack_mgetl */
static void toluaI_pack_mgetl0(void)
{
 if (
     !tolua_istype(1,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(1,0));
  {
   long toluaI_ret = pack_mgetl(f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_mgetl'.");
}

/* function: pack_mputw */
static void toluaI_pack_mputw0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  int w = ((int)  tolua_getnumber(1,0));
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(2,0));
  {
   int toluaI_ret = pack_mputw(w,f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_mputw'.");
}

/* function: pack_mputl */
static void toluaI_pack_mputl0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  long l = ((long)  tolua_getnumber(1,0));
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(2,0));
  {
   long toluaI_ret = pack_mputl(l,f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_mputl'.");
}

/* function: pack_fread */
static void toluaI_pack_fread0(void)
{
 if (
     !tolua_istype(1,tolua_tag_userdata,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  void* p = ((void*)  tolua_getuserdata(1,0));
  long n = ((long)  tolua_getnumber(2,0));
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(3,0));
  {
   long toluaI_ret = pack_fread(p,n,f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fread'.");
}

/* function: pack_fwrite */
static void toluaI_pack_fwrite0(void)
{
 if (
     !tolua_istype(1,tolua_tag_userdata,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  void* p = ((void*)  tolua_getuserdata(1,0));
  long n = ((long)  tolua_getnumber(2,0));
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(3,0));
  {
   long toluaI_ret = pack_fwrite(p,n,f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fwrite'.");
}

/* function: pack_fgets */
static void toluaI_pack_fgets0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_istype(3,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* p = ((char*)  tolua_getstring(1,0));
  int max = ((int)  tolua_getnumber(2,0));
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(3,0));
  {
   char* toluaI_ret = pack_fgets(p,max,f);
   tolua_pushstring(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fgets'.");
}

/* function: pack_fputs */
static void toluaI_pack_fputs0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_PACKFILE,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* p = ((char*)  tolua_getstring(1,0));
  PACKFILE* f = ((PACKFILE*)  tolua_getusertype(2,0));
  {
   int toluaI_ret = pack_fputs(p,f);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'pack_fputs'.");
}

/* function: load_datafile */
static void toluaI_load_datafile0(void)
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
   DATAFILE* toluaI_ret = load_datafile(filename);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_DATAFILE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_datafile'.");
}

/* function: unload_datafile */
static void toluaI_unload_datafile0(void)
{
 if (
     !tolua_istype(1,tolua_tag_DATAFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  DATAFILE* dat = ((DATAFILE*)  tolua_getusertype(1,0));
  {
   unload_datafile(dat);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'unload_datafile'.");
}

/* function: load_datafile_object */
static void toluaI_load_datafile_object0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  char* objectname = ((char*)  tolua_getstring(2,0));
  {
   DATAFILE* toluaI_ret = load_datafile_object(filename,objectname);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_DATAFILE);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_datafile_object'.");
}

/* function: unload_datafile_object */
static void toluaI_unload_datafile_object0(void)
{
 if (
     !tolua_istype(1,tolua_tag_DATAFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  DATAFILE* dat = ((DATAFILE*)  tolua_getusertype(1,0));
  {
   unload_datafile_object(dat);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'unload_datafile_object'.");
}

/* function: get_datafile_property */
static void toluaI_get_datafile_property0(void)
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
   char* toluaI_ret = get_datafile_property(dat,type);
   tolua_pushstring(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'get_datafile_property'.");
}

/* function: fixup_datafile */
static void toluaI_fixup_datafile0(void)
{
 if (
     !tolua_istype(1,tolua_tag_DATAFILE,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  DATAFILE* data = ((DATAFILE*)  tolua_getusertype(1,0));
  {
   fixup_datafile(data);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fixup_datafile'.");
}

/* function: __ugly_but_true__dat */
static void toluaI_dat0(void)
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
   void* toluaI_ret = __ugly_but_true__dat(d,i);
   tolua_pushuserdata(toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'dat'.");
}

/* function: load_bitmap */
static void toluaI_load_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_RGB,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  RGB* pal = ((RGB*)  tolua_getusertype(2,0));
  {
   BITMAP* toluaI_ret = load_bitmap(filename,pal);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_bitmap'.");
}

/* function: load_bmp */
static void toluaI_load_bmp0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_RGB,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  RGB* pal = ((RGB*)  tolua_getusertype(2,0));
  {
   BITMAP* toluaI_ret = load_bmp(filename,pal);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_bmp'.");
}

/* function: load_lbm */
static void toluaI_load_lbm0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_RGB,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  RGB* pal = ((RGB*)  tolua_getusertype(2,0));
  {
   BITMAP* toluaI_ret = load_lbm(filename,pal);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_lbm'.");
}

/* function: load_pcx */
static void toluaI_load_pcx0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_RGB,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  RGB* pal = ((RGB*)  tolua_getusertype(2,0));
  {
   BITMAP* toluaI_ret = load_pcx(filename,pal);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_pcx'.");
}

/* function: load_tga */
static void toluaI_load_tga0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_RGB,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  RGB* pal = ((RGB*)  tolua_getusertype(2,0));
  {
   BITMAP* toluaI_ret = load_tga(filename,pal);
   tolua_pushusertype((void*)toluaI_ret,tolua_tag_BITMAP);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'load_tga'.");
}

/* function: save_bitmap */
static void toluaI_save_bitmap0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_RGB,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(2,0));
  RGB* pal = ((RGB*)  tolua_getusertype(3,0));
  {
   int toluaI_ret = save_bitmap(filename,bmp,pal);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'save_bitmap'.");
}

/* function: save_bmp */
static void toluaI_save_bmp0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_RGB,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(2,0));
  RGB* pal = ((RGB*)  tolua_getusertype(3,0));
  {
   int toluaI_ret = save_bmp(filename,bmp,pal);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'save_bmp'.");
}

/* function: save_pcx */
static void toluaI_save_pcx0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_RGB,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(2,0));
  RGB* pal = ((RGB*)  tolua_getusertype(3,0));
  {
   int toluaI_ret = save_pcx(filename,bmp,pal);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'save_pcx'.");
}

/* function: save_tga */
static void toluaI_save_tga0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_BITMAP,0) ||
     !tolua_istype(3,tolua_tag_RGB,0) ||
     !tolua_isnoobj(4)
 )
  goto error;
 else
 {
  char* filename = ((char*)  tolua_getstring(1,0));
  BITMAP* bmp = ((BITMAP*)  tolua_getusertype(2,0));
  RGB* pal = ((RGB*)  tolua_getusertype(3,0));
  {
   int toluaI_ret = save_tga(filename,bmp,pal);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'save_tga'.");
}

/* function: itofix */
static void toluaI_itofix0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  int x = ((int)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = itofix(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'itofix'.");
}

/* function: fixtoi */
static void toluaI_fixtoi0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   int toluaI_ret = fixtoi(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fixtoi'.");
}

/* function: ftofix */
static void toluaI_ftofix0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  double x = ((double)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = ftofix(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'ftofix'.");
}

/* function: fixtof */
static void toluaI_fixtof0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   double toluaI_ret = fixtof(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fixtof'.");
}

/* function: fsqrt */
static void toluaI_fsqrt0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = fsqrt(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fsqrt'.");
}

/* function: fatan */
static void toluaI_fatan0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = fatan(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fatan'.");
}

/* function: fatan2 */
static void toluaI_fatan20(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  fixed y = ((fixed)  tolua_getnumber(1,0));
  fixed x = ((fixed)  tolua_getnumber(2,0));
  {
   fixed toluaI_ret = fatan2(y,x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fatan2'.");
}

/* function: fcos */
static void toluaI_fcos0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = fcos(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fcos'.");
}

/* function: fsin */
static void toluaI_fsin0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = fsin(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fsin'.");
}

/* function: ftan */
static void toluaI_ftan0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = ftan(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'ftan'.");
}

/* function: facos */
static void toluaI_facos0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = facos(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'facos'.");
}

/* function: fasin */
static void toluaI_fasin0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_isnoobj(2)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  {
   fixed toluaI_ret = fasin(x);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fasin'.");
}

/* function: fadd */
static void toluaI_fadd0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  fixed y = ((fixed)  tolua_getnumber(2,0));
  {
   fixed toluaI_ret = fadd(x,y);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fadd'.");
}

/* function: fsub */
static void toluaI_fsub0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  fixed y = ((fixed)  tolua_getnumber(2,0));
  {
   fixed toluaI_ret = fsub(x,y);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fsub'.");
}

/* function: fmul */
static void toluaI_fmul0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  fixed y = ((fixed)  tolua_getnumber(2,0));
  {
   fixed toluaI_ret = fmul(x,y);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fmul'.");
}

/* function: fdiv */
static void toluaI_fdiv0(void)
{
 if (
     !tolua_istype(1,tolua_tag_number,0) ||
     !tolua_istype(2,tolua_tag_number,0) ||
     !tolua_isnoobj(3)
 )
  goto error;
 else
 {
  fixed x = ((fixed)  tolua_getnumber(1,0));
  fixed y = ((fixed)  tolua_getnumber(2,0));
  {
   fixed toluaI_ret = fdiv(x,y);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'fdiv'.");
}

/* function: alert */
static void toluaI_alert0(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_string,0) ||
     !tolua_istype(4,tolua_tag_string,0) ||
     !tolua_istype(5,tolua_tag_string,0) ||
     !tolua_istype(6,tolua_tag_number,0) ||
     !tolua_istype(7,tolua_tag_number,0) ||
     !tolua_isnoobj(8)
 )
  goto error;
 else
 {
  char* s1 = ((char*)  tolua_getstring(1,0));
  char* s2 = ((char*)  tolua_getstring(2,0));
  char* s3 = ((char*)  tolua_getstring(3,0));
  char* b1 = ((char*)  tolua_getstring(4,0));
  char* b2 = ((char*)  tolua_getstring(5,0));
  int c1 = ((int)  tolua_getnumber(6,0));
  int c2 = ((int)  tolua_getnumber(7,0));
  {
   int toluaI_ret = alert(s1,s2,s3,b1,b2,c1,c2);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'alert'.");
}

/* function: alert3 */
static void toluaI_alert30(void)
{
 if (
     !tolua_istype(1,tolua_tag_string,0) ||
     !tolua_istype(2,tolua_tag_string,0) ||
     !tolua_istype(3,tolua_tag_string,0) ||
     !tolua_istype(4,tolua_tag_string,0) ||
     !tolua_istype(5,tolua_tag_string,0) ||
     !tolua_istype(6,tolua_tag_string,0) ||
     !tolua_istype(7,tolua_tag_number,0) ||
     !tolua_istype(8,tolua_tag_number,0) ||
     !tolua_istype(9,tolua_tag_number,0) ||
     !tolua_isnoobj(10)
 )
  goto error;
 else
 {
  char* s1 = ((char*)  tolua_getstring(1,0));
  char* s2 = ((char*)  tolua_getstring(2,0));
  char* s3 = ((char*)  tolua_getstring(3,0));
  char* b1 = ((char*)  tolua_getstring(4,0));
  char* b2 = ((char*)  tolua_getstring(5,0));
  char* b3 = ((char*)  tolua_getstring(6,0));
  int c1 = ((int)  tolua_getnumber(7,0));
  int c2 = ((int)  tolua_getnumber(8,0));
  int c3 = ((int)  tolua_getnumber(9,0));
  {
   int toluaI_ret = alert3(s1,s2,s3,b1,b2,b3,c1,c2,c3);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'alert3'.");
}

/* function: file_select */
static void toluaI_file_select0(void)
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
  char* message = ((char*)  tolua_getstring(1,0));
  char* path = ((char*)  tolua_getstring(2,0));
  char* ext = ((char*)  tolua_getstring(3,0));
  {
   int toluaI_ret = file_select(message,path,ext);
   tolua_pushnumber((double)toluaI_ret);
  }
 }
 return;
error:
 tolua_error("#ferror in function 'file_select'.");
}

/* Initialization function */
int tolua_allegro_open (void)
{
 tolua_open();
 toluaI_init_tag();
 tolua_cclass("RGB","");
 tolua_cclass("BITMAP","");
 tolua_cclass("FONT","");
 tolua_cclass("SAMPLE","");
 tolua_cclass("MIDI","");
 tolua_cclass("AUDIOSTREAM","");
 tolua_cclass("PACKFILE","");
 tolua_cclass("DATAFILE","");
 tolua_tablevar("DATAFILE","dat",toluaI_get_DATAFILE_dat,toluaI_set_DATAFILE_dat);
 tolua_constant(NULL,"TRUE",TRUE);
 tolua_constant(NULL,"FALSE",FALSE);
 tolua_constant(NULL,"MOUSE_FLAG_MOVE",MOUSE_FLAG_MOVE);
 tolua_constant(NULL,"MOUSE_FLAG_LEFT_DOWN",MOUSE_FLAG_LEFT_DOWN);
 tolua_constant(NULL,"MOUSE_FLAG_LEFT_UP",MOUSE_FLAG_LEFT_UP);
 tolua_constant(NULL,"MOUSE_FLAG_RIGHT_DOWN",MOUSE_FLAG_RIGHT_DOWN);
 tolua_constant(NULL,"MOUSE_FLAG_RIGHT_UP",MOUSE_FLAG_RIGHT_UP);
 tolua_constant(NULL,"MOUSE_FLAG_MIDDLE_DOWN",MOUSE_FLAG_MIDDLE_DOWN);
 tolua_constant(NULL,"MOUSE_FLAG_MIDDLE_UP",MOUSE_FLAG_MIDDLE_UP);
 tolua_constant(NULL,"KB_SHIFT_FLAG",KB_SHIFT_FLAG);
 tolua_constant(NULL,"KB_CTRL_FLAG",KB_CTRL_FLAG);
 tolua_constant(NULL,"KB_ALT_FLAG",KB_ALT_FLAG);
 tolua_constant(NULL,"KB_LWIN_FLAG",KB_LWIN_FLAG);
 tolua_constant(NULL,"KB_RWIN_FLAG",KB_RWIN_FLAG);
 tolua_constant(NULL,"KB_MENU_FLAG",KB_MENU_FLAG);
 tolua_constant(NULL,"KB_SCROLOCK_FLAG",KB_SCROLOCK_FLAG);
 tolua_constant(NULL,"KB_NUMLOCK_FLAG",KB_NUMLOCK_FLAG);
 tolua_constant(NULL,"KB_CAPSLOCK_FLAG",KB_CAPSLOCK_FLAG);
 tolua_constant(NULL,"KB_INALTSEQ_FLAG",KB_INALTSEQ_FLAG);
 tolua_constant(NULL,"KB_ACCENT1_FLAG",KB_ACCENT1_FLAG);
 tolua_constant(NULL,"KB_ACCENT1_S_FLAG",KB_ACCENT1_S_FLAG);
 tolua_constant(NULL,"KB_ACCENT2_FLAG",KB_ACCENT2_FLAG);
 tolua_constant(NULL,"KB_ACCENT2_S_FLAG",KB_ACCENT2_S_FLAG);
 tolua_constant(NULL,"KB_NORMAL",KB_NORMAL);
 tolua_constant(NULL,"KB_EXTENDED",KB_EXTENDED);
 tolua_constant(NULL,"KEY_ESC",KEY_ESC);
 tolua_constant(NULL,"KEY_1",KEY_1);
 tolua_constant(NULL,"KEY_2",KEY_2);
 tolua_constant(NULL,"KEY_3",KEY_3);
 tolua_constant(NULL,"KEY_4",KEY_4);
 tolua_constant(NULL,"KEY_5",KEY_5);
 tolua_constant(NULL,"KEY_6",KEY_6);
 tolua_constant(NULL,"KEY_7",KEY_7);
 tolua_constant(NULL,"KEY_8",KEY_8);
 tolua_constant(NULL,"KEY_9",KEY_9);
 tolua_constant(NULL,"KEY_0",KEY_0);
 tolua_constant(NULL,"KEY_MINUS",KEY_MINUS);
 tolua_constant(NULL,"KEY_EQUALS",KEY_EQUALS);
 tolua_constant(NULL,"KEY_BACKSPACE",KEY_BACKSPACE);
 tolua_constant(NULL,"KEY_TAB",KEY_TAB);
 tolua_constant(NULL,"KEY_Q",KEY_Q);
 tolua_constant(NULL,"KEY_W",KEY_W);
 tolua_constant(NULL,"KEY_E",KEY_E);
 tolua_constant(NULL,"KEY_R",KEY_R);
 tolua_constant(NULL,"KEY_T",KEY_T);
 tolua_constant(NULL,"KEY_Y",KEY_Y);
 tolua_constant(NULL,"KEY_U",KEY_U);
 tolua_constant(NULL,"KEY_I",KEY_I);
 tolua_constant(NULL,"KEY_O",KEY_O);
 tolua_constant(NULL,"KEY_P",KEY_P);
 tolua_constant(NULL,"KEY_OPENBRACE",KEY_OPENBRACE);
 tolua_constant(NULL,"KEY_CLOSEBRACE",KEY_CLOSEBRACE);
 tolua_constant(NULL,"KEY_ENTER",KEY_ENTER);
 tolua_constant(NULL,"KEY_CONTROL",KEY_CONTROL);
 tolua_constant(NULL,"KEY_LCONTROL",KEY_LCONTROL);
 tolua_constant(NULL,"KEY_A",KEY_A);
 tolua_constant(NULL,"KEY_S",KEY_S);
 tolua_constant(NULL,"KEY_D",KEY_D);
 tolua_constant(NULL,"KEY_F",KEY_F);
 tolua_constant(NULL,"KEY_G",KEY_G);
 tolua_constant(NULL,"KEY_H",KEY_H);
 tolua_constant(NULL,"KEY_J",KEY_J);
 tolua_constant(NULL,"KEY_K",KEY_K);
 tolua_constant(NULL,"KEY_L",KEY_L);
 tolua_constant(NULL,"KEY_COLON",KEY_COLON);
 tolua_constant(NULL,"KEY_QUOTE",KEY_QUOTE);
 tolua_constant(NULL,"KEY_TILDE",KEY_TILDE);
 tolua_constant(NULL,"KEY_LSHIFT",KEY_LSHIFT);
 tolua_constant(NULL,"KEY_BACKSLASH",KEY_BACKSLASH);
 tolua_constant(NULL,"KEY_Z",KEY_Z);
 tolua_constant(NULL,"KEY_X",KEY_X);
 tolua_constant(NULL,"KEY_C",KEY_C);
 tolua_constant(NULL,"KEY_V",KEY_V);
 tolua_constant(NULL,"KEY_B",KEY_B);
 tolua_constant(NULL,"KEY_N",KEY_N);
 tolua_constant(NULL,"KEY_M",KEY_M);
 tolua_constant(NULL,"KEY_COMMA",KEY_COMMA);
 tolua_constant(NULL,"KEY_STOP",KEY_STOP);
 tolua_constant(NULL,"KEY_SLASH",KEY_SLASH);
 tolua_constant(NULL,"KEY_RSHIFT",KEY_RSHIFT);
 tolua_constant(NULL,"KEY_ASTERISK",KEY_ASTERISK);
 tolua_constant(NULL,"KEY_ALT",KEY_ALT);
 tolua_constant(NULL,"KEY_SPACE",KEY_SPACE);
 tolua_constant(NULL,"KEY_CAPSLOCK",KEY_CAPSLOCK);
 tolua_constant(NULL,"KEY_F1",KEY_F1);
 tolua_constant(NULL,"KEY_F2",KEY_F2);
 tolua_constant(NULL,"KEY_F3",KEY_F3);
 tolua_constant(NULL,"KEY_F4",KEY_F4);
 tolua_constant(NULL,"KEY_F5",KEY_F5);
 tolua_constant(NULL,"KEY_F6",KEY_F6);
 tolua_constant(NULL,"KEY_F7",KEY_F7);
 tolua_constant(NULL,"KEY_F8",KEY_F8);
 tolua_constant(NULL,"KEY_F9",KEY_F9);
 tolua_constant(NULL,"KEY_F10",KEY_F10);
 tolua_constant(NULL,"KEY_NUMLOCK",KEY_NUMLOCK);
 tolua_constant(NULL,"KEY_SCRLOCK",KEY_SCRLOCK);
 tolua_constant(NULL,"KEY_HOME",KEY_HOME);
 tolua_constant(NULL,"KEY_UP",KEY_UP);
 tolua_constant(NULL,"KEY_PGUP",KEY_PGUP);
 tolua_constant(NULL,"KEY_MINUS_PAD",KEY_MINUS_PAD);
 tolua_constant(NULL,"KEY_LEFT",KEY_LEFT);
 tolua_constant(NULL,"KEY_5_PAD",KEY_5_PAD);
 tolua_constant(NULL,"KEY_RIGHT",KEY_RIGHT);
 tolua_constant(NULL,"KEY_PLUS_PAD",KEY_PLUS_PAD);
 tolua_constant(NULL,"KEY_END",KEY_END);
 tolua_constant(NULL,"KEY_DOWN",KEY_DOWN);
 tolua_constant(NULL,"KEY_PGDN",KEY_PGDN);
 tolua_constant(NULL,"KEY_INSERT",KEY_INSERT);
 tolua_constant(NULL,"KEY_DEL",KEY_DEL);
 tolua_constant(NULL,"KEY_PRTSCR",KEY_PRTSCR);
 tolua_constant(NULL,"KEY_F11",KEY_F11);
 tolua_constant(NULL,"KEY_F12",KEY_F12);
 tolua_constant(NULL,"KEY_LWIN",KEY_LWIN);
 tolua_constant(NULL,"KEY_RWIN",KEY_RWIN);
 tolua_constant(NULL,"KEY_MENU",KEY_MENU);
 tolua_constant(NULL,"KEY_PAD",KEY_PAD);
 tolua_constant(NULL,"KEY_RCONTROL",KEY_RCONTROL);
 tolua_constant(NULL,"KEY_ALTGR",KEY_ALTGR);
 tolua_constant(NULL,"KEY_SLASH2",KEY_SLASH2);
 tolua_constant(NULL,"KEY_PAUSE",KEY_PAUSE);
 tolua_constant(NULL,"MASK_COLOR_8",MASK_COLOR_8);
 tolua_constant(NULL,"MASK_COLOR_15",MASK_COLOR_15);
 tolua_constant(NULL,"MASK_COLOR_16",MASK_COLOR_16);
 tolua_constant(NULL,"MASK_COLOR_24",MASK_COLOR_24);
 tolua_constant(NULL,"MASK_COLOR_32",MASK_COLOR_32);
 tolua_constant(NULL,"DRAW_MODE_SOLID",DRAW_MODE_SOLID);
 tolua_constant(NULL,"DRAW_MODE_XOR",DRAW_MODE_XOR);
 tolua_constant(NULL,"DRAW_MODE_COPY_PATTERN",DRAW_MODE_COPY_PATTERN);
 tolua_constant(NULL,"DRAW_MODE_SOLID_PATTERN",DRAW_MODE_SOLID_PATTERN);
 tolua_constant(NULL,"DRAW_MODE_MASKED_PATTERN",DRAW_MODE_MASKED_PATTERN);
 tolua_constant(NULL,"DRAW_MODE_TRANS",DRAW_MODE_TRANS);
 tolua_constant(NULL,"EOF",EOF);
 tolua_function(NULL,"BITMAP",toluaI_BITMAP0);
 tolua_function(NULL,"set_config_file",toluaI_set_config_file0);
 tolua_function(NULL,"set_config_data",toluaI_set_config_data0);
 tolua_function(NULL,"override_config_file",toluaI_override_config_file0);
 tolua_function(NULL,"override_config_data",toluaI_override_config_data0);
 tolua_function(NULL,"push_config_state",toluaI_push_config_state0);
 tolua_function(NULL,"pop_config_state",toluaI_pop_config_state0);
 tolua_function(NULL,"get_config_string",toluaI_get_config_string0);
 tolua_function(NULL,"get_config_int",toluaI_get_config_int0);
 tolua_function(NULL,"get_config_hex",toluaI_get_config_hex0);
 tolua_function(NULL,"get_config_float",toluaI_get_config_float0);
 tolua_function(NULL,"set_config_string",toluaI_set_config_string0);
 tolua_function(NULL,"set_config_int",toluaI_set_config_int0);
 tolua_function(NULL,"set_config_hex",toluaI_set_config_hex0);
 tolua_function(NULL,"set_config_float",toluaI_set_config_float0);
 tolua_globalvar("mouse_x",toluaI_get_mouse_x,toluaI_set_mouse_x);
 tolua_globalvar("mouse_y",toluaI_get_mouse_y,toluaI_set_mouse_y);
 tolua_globalvar("mouse_b",toluaI_get_mouse_b,toluaI_set_mouse_b);
 tolua_globalvar("mouse_pos",toluaI_get_mouse_pos,toluaI_set_mouse_pos);
 tolua_globalvar("freeze_mouse_flag",toluaI_get_freeze_mouse_flag,toluaI_set_freeze_mouse_flag);
 tolua_function(NULL,"show_mouse",toluaI_show_mouse0);
 tolua_function(NULL,"position_mouse",toluaI_position_mouse0);
 tolua_function(NULL,"set_mouse_range",toluaI_set_mouse_range0);
 tolua_function(NULL,"set_mouse_speed",toluaI_set_mouse_speed0);
 tolua_function(NULL,"set_mouse_sprite",toluaI_set_mouse_sprite0);
 tolua_function(NULL,"set_mouse_sprite_focus",toluaI_set_mouse_sprite_focus0);
 tolua_function(NULL,"get_mouse_mickeys",toluaI_get_mouse_mickeys0);
 tolua_function(NULL,"rest",toluaI_rest0);
 tolua_function(NULL,"key",toluaI_key0);
 tolua_globalvar("key_shifts",toluaI_get_key_shifts,toluaI_set_key_shifts);
 tolua_globalvar("three_finger_flag",toluaI_get_three_finger_flag,toluaI_set_three_finger_flag);
 tolua_globalvar("key_led_flag",toluaI_get_key_led_flag,toluaI_set_key_led_flag);
 tolua_function(NULL,"keypressed",toluaI_keypressed0);
 tolua_function(NULL,"readkey",toluaI_readkey0);
 tolua_function(NULL,"simulate_keypress",toluaI_simulate_keypress0);
 tolua_function(NULL,"clear_keybuf",toluaI_clear_keybuf0);
 tolua_function(NULL,"set_leds",toluaI_set_leds0);
 tolua_globalvar("joy_type",toluaI_get_joy_type,toluaI_set_joy_type);
 tolua_constant(NULL,"JOY_TYPE_STANDARD",JOY_TYPE_STANDARD);
 tolua_constant(NULL,"JOY_TYPE_FSPRO",JOY_TYPE_FSPRO);
 tolua_constant(NULL,"JOY_TYPE_4BUTTON",JOY_TYPE_4BUTTON);
 tolua_constant(NULL,"JOY_TYPE_6BUTTON",JOY_TYPE_6BUTTON);
 tolua_constant(NULL,"JOY_TYPE_2PADS",JOY_TYPE_2PADS);
 tolua_constant(NULL,"JOY_TYPE_WINGEX",JOY_TYPE_WINGEX);
 tolua_constant(NULL,"JOY_HAT_CENTRE",JOY_HAT_CENTRE);
 tolua_constant(NULL,"JOY_HAT_LEFT",JOY_HAT_LEFT);
 tolua_constant(NULL,"JOY_HAT_DOWN",JOY_HAT_DOWN);
 tolua_constant(NULL,"JOY_HAT_RIGHT",JOY_HAT_RIGHT);
 tolua_constant(NULL,"JOY_HAT_UP",JOY_HAT_UP);
 tolua_constant(NULL,"joy_FSPRO_trigger",joy_FSPRO_trigger);
 tolua_constant(NULL,"joy_FSPRO_butleft",joy_FSPRO_butleft);
 tolua_constant(NULL,"joy_FSPRO_butright",joy_FSPRO_butright);
 tolua_constant(NULL,"joy_FSPRO_butmiddle",joy_FSPRO_butmiddle);
 tolua_constant(NULL,"joy_WINGEX_trigger",joy_WINGEX_trigger);
 tolua_constant(NULL,"joy_WINGEX_buttop",joy_WINGEX_buttop);
 tolua_constant(NULL,"joy_WINGEX_butthumb",joy_WINGEX_butthumb);
 tolua_constant(NULL,"joy_WINGEX_butmiddle",joy_WINGEX_butmiddle);
 tolua_constant(NULL,"JOY_HAT_CENTER",JOY_HAT_CENTER);
 tolua_globalvar("joy_x",toluaI_get_joy_x,toluaI_set_joy_x);
 tolua_globalvar("joy_y",toluaI_get_joy_y,toluaI_set_joy_y);
 tolua_globalvar("joy_left",toluaI_get_joy_left,toluaI_set_joy_left);
 tolua_globalvar("joy_right",toluaI_get_joy_right,toluaI_set_joy_right);
 tolua_globalvar("joy_up",toluaI_get_joy_up,toluaI_set_joy_up);
 tolua_globalvar("joy_down",toluaI_get_joy_down,toluaI_set_joy_down);
 tolua_globalvar("joy_b1",toluaI_get_joy_b1,toluaI_set_joy_b1);
 tolua_globalvar("joy_b2",toluaI_get_joy_b2,toluaI_set_joy_b2);
 tolua_globalvar("joy_b3",toluaI_get_joy_b3,toluaI_set_joy_b3);
 tolua_globalvar("joy_b4",toluaI_get_joy_b4,toluaI_set_joy_b4);
 tolua_globalvar("joy_b5",toluaI_get_joy_b5,toluaI_set_joy_b5);
 tolua_globalvar("joy_b6",toluaI_get_joy_b6,toluaI_set_joy_b6);
 tolua_globalvar("joy_hat",toluaI_get_joy_hat,toluaI_set_joy_hat);
 tolua_globalvar("joy_throttle",toluaI_get_joy_throttle,toluaI_set_joy_throttle);
 tolua_globalvar("joy2_x",toluaI_get_joy2_x,toluaI_set_joy2_x);
 tolua_globalvar("joy2_y",toluaI_get_joy2_y,toluaI_set_joy2_y);
 tolua_globalvar("joy2_left",toluaI_get_joy2_left,toluaI_set_joy2_left);
 tolua_globalvar("joy2_right",toluaI_get_joy2_right,toluaI_set_joy2_right);
 tolua_globalvar("joy2_up",toluaI_get_joy2_up,toluaI_set_joy2_up);
 tolua_globalvar("joy2_down",toluaI_get_joy2_down,toluaI_set_joy2_down);
 tolua_globalvar("joy2_b1",toluaI_get_joy2_b1,toluaI_set_joy2_b1);
 tolua_globalvar("joy2_b2",toluaI_get_joy2_b2,toluaI_set_joy2_b2);
 tolua_function(NULL,"initialise_joystick",toluaI_initialise_joystick0);
 tolua_function(NULL,"calibrate_joystick_tl",toluaI_calibrate_joystick_tl0);
 tolua_function(NULL,"calibrate_joystick_br",toluaI_calibrate_joystick_br0);
 tolua_function(NULL,"calibrate_joystick_throttle_min",toluaI_calibrate_joystick_throttle_min0);
 tolua_function(NULL,"calibrate_joystick_throttle_max",toluaI_calibrate_joystick_throttle_max0);
 tolua_function(NULL,"calibrate_joystick_hat",toluaI_calibrate_joystick_hat0);
 tolua_function(NULL,"poll_joystick",toluaI_poll_joystick0);
 tolua_function(NULL,"save_joystick_data",toluaI_save_joystick_data0);
 tolua_function(NULL,"load_joystick_data",toluaI_load_joystick_data0);
 tolua_globalvar("screen",toluaI_get_screen,toluaI_set_screen);
 tolua_function(NULL,"set_color_conversion",toluaI_set_color_conversion0);
 tolua_function(NULL,"set_colour_conversion",toluaI_set_colour_conversion0);
 tolua_function(NULL,"create_bitmap",toluaI_create_bitmap0);
 tolua_function(NULL,"create_bitmap_ex",toluaI_create_bitmap_ex0);
 tolua_function(NULL,"create_sub_bitmap",toluaI_create_sub_bitmap0);
 tolua_function(NULL,"destroy_bitmap",toluaI_destroy_bitmap0);
 tolua_function(NULL,"bitmap_color_depth",toluaI_bitmap_color_depth0);
 tolua_function(NULL,"bitmap_mask_color",toluaI_bitmap_mask_color0);
 tolua_function(NULL,"is_same_bitmap",toluaI_is_same_bitmap0);
 tolua_function(NULL,"is_linear_bitmap",toluaI_is_linear_bitmap0);
 tolua_function(NULL,"is_planar_bitmap",toluaI_is_planar_bitmap0);
 tolua_function(NULL,"is_memory_bitmap",toluaI_is_memory_bitmap0);
 tolua_function(NULL,"is_screen_bitmap",toluaI_is_screen_bitmap0);
 tolua_function(NULL,"is_sub_bitmap",toluaI_is_sub_bitmap0);
 tolua_globalvar("_color_depth",toluaI_get__color_depth,toluaI_set__color_depth);
 tolua_function(NULL,"set_color",toluaI_set_color0);
 tolua_function(NULL,"set_colour",toluaI_set_colour0);
 tolua_function(NULL,"get_color",toluaI_get_color0);
 tolua_function(NULL,"fade_out_range",toluaI_fade_out_range0);
 tolua_function(NULL,"fade_out",toluaI_fade_out0);
 tolua_function(NULL,"unselect_pallete",toluaI_unselect_pallete0);
 tolua_function(NULL,"hsv_to_rgb",toluaI_hsv_to_rgb0);
 tolua_function(NULL,"rgb_to_hsv",toluaI_rgb_to_hsv0);
 tolua_function(NULL,"makecol",toluaI_makecol0);
 tolua_function(NULL,"makecol8",toluaI_makecol80);
 tolua_function(NULL,"makecol15",toluaI_makecol150);
 tolua_function(NULL,"makecol16",toluaI_makecol160);
 tolua_function(NULL,"makecol24",toluaI_makecol240);
 tolua_function(NULL,"makecol32",toluaI_makecol320);
 tolua_function(NULL,"makecol_depth",toluaI_makecol_depth0);
 tolua_function(NULL,"getr",toluaI_getr0);
 tolua_function(NULL,"getg",toluaI_getg0);
 tolua_function(NULL,"getb",toluaI_getb0);
 tolua_function(NULL,"getr8",toluaI_getr80);
 tolua_function(NULL,"getg8",toluaI_getg80);
 tolua_function(NULL,"getb8",toluaI_getb80);
 tolua_function(NULL,"getr15",toluaI_getr150);
 tolua_function(NULL,"getg15",toluaI_getg150);
 tolua_function(NULL,"getb15",toluaI_getb150);
 tolua_function(NULL,"getr16",toluaI_getr160);
 tolua_function(NULL,"getg16",toluaI_getg160);
 tolua_function(NULL,"getb16",toluaI_getb160);
 tolua_function(NULL,"getr24",toluaI_getr240);
 tolua_function(NULL,"getg24",toluaI_getg240);
 tolua_function(NULL,"getb24",toluaI_getb240);
 tolua_function(NULL,"getr32",toluaI_getr320);
 tolua_function(NULL,"getg32",toluaI_getg320);
 tolua_function(NULL,"getb32",toluaI_getb320);
 tolua_function(NULL,"getr_depth",toluaI_getr_depth0);
 tolua_function(NULL,"getg_depth",toluaI_getg_depth0);
 tolua_function(NULL,"getb_depth",toluaI_getb_depth0);
 tolua_function(NULL,"set_clip",toluaI_set_clip0);
 tolua_function(NULL,"drawing_mode",toluaI_drawing_mode0);
 tolua_function(NULL,"xor_mode",toluaI_xor_mode0);
 tolua_function(NULL,"solid_mode",toluaI_solid_mode0);
 tolua_function(NULL,"getpixel",toluaI_getpixel0);
 tolua_function(NULL,"putpixel",toluaI_putpixel0);
 tolua_function(NULL,"vline",toluaI_vline0);
 tolua_function(NULL,"hline",toluaI_hline0);
 tolua_function(NULL,"line",toluaI_line0);
 tolua_function(NULL,"rectfill",toluaI_rectfill0);
 tolua_function(NULL,"draw_sprite",toluaI_draw_sprite0);
 tolua_function(NULL,"draw_sprite_v_flip",toluaI_draw_sprite_v_flip0);
 tolua_function(NULL,"draw_sprite_h_flip",toluaI_draw_sprite_h_flip0);
 tolua_function(NULL,"draw_sprite_vh_flip",toluaI_draw_sprite_vh_flip0);
 tolua_function(NULL,"draw_trans_sprite",toluaI_draw_trans_sprite0);
 tolua_function(NULL,"draw_lit_sprite",toluaI_draw_lit_sprite0);
 tolua_function(NULL,"draw_character",toluaI_draw_character0);
 tolua_function(NULL,"clear_to_color",toluaI_clear_to_color0);
 tolua_function(NULL,"triangle",toluaI_triangle0);
 tolua_function(NULL,"polygon",toluaI_polygon0);
 tolua_function(NULL,"rect",toluaI_rect0);
 tolua_function(NULL,"circle",toluaI_circle0);
 tolua_function(NULL,"circlefill",toluaI_circlefill0);
 tolua_function(NULL,"ellipse",toluaI_ellipse0);
 tolua_function(NULL,"ellipsefill",toluaI_ellipsefill0);
 tolua_function(NULL,"calc_spline",toluaI_calc_spline0);
 tolua_function(NULL,"spline",toluaI_spline0);
 tolua_function(NULL,"floodfill",toluaI_floodfill0);
 tolua_function(NULL,"blit",toluaI_blit0);
 tolua_function(NULL,"masked_blit",toluaI_masked_blit0);
 tolua_function(NULL,"stretch_blit",toluaI_stretch_blit0);
 tolua_function(NULL,"stretch_sprite",toluaI_stretch_sprite0);
 tolua_function(NULL,"rotate_sprite",toluaI_rotate_sprite0);
 tolua_function(NULL,"rotate_scaled_sprite",toluaI_rotate_scaled_sprite0);
 tolua_function(NULL,"clear",toluaI_clear0);
 tolua_globalvar("font",toluaI_get_font,toluaI_set_font);
 tolua_function(NULL,"text_mode",toluaI_text_mode0);
 tolua_function(NULL,"textout",toluaI_textout0);
 tolua_function(NULL,"textout_centre",toluaI_textout_centre0);
 tolua_function(NULL,"textout_justify",toluaI_textout_justify0);
 tolua_function(NULL,"text_length",toluaI_text_length0);
 tolua_function(NULL,"text_height",toluaI_text_height0);
 tolua_function(NULL,"destroy_font",toluaI_destroy_font0);
 tolua_function(NULL,"reserve_voices",toluaI_reserve_voices0);
 tolua_function(NULL,"set_volume",toluaI_set_volume0);
 tolua_function(NULL,"load_sample",toluaI_load_sample0);
 tolua_function(NULL,"load_wav",toluaI_load_wav0);
 tolua_function(NULL,"load_voc",toluaI_load_voc0);
 tolua_function(NULL,"destroy_sample",toluaI_destroy_sample0);
 tolua_function(NULL,"play_sample",toluaI_play_sample0);
 tolua_function(NULL,"stop_sample",toluaI_stop_sample0);
 tolua_function(NULL,"adjust_sample",toluaI_adjust_sample0);
 tolua_function(NULL,"allocate_voice",toluaI_allocate_voice0);
 tolua_function(NULL,"deallocate_voice",toluaI_deallocate_voice0);
 tolua_function(NULL,"reallocate_voice",toluaI_reallocate_voice0);
 tolua_function(NULL,"release_voice",toluaI_release_voice0);
 tolua_function(NULL,"voice_start",toluaI_voice_start0);
 tolua_function(NULL,"voice_stop",toluaI_voice_stop0);
 tolua_function(NULL,"voice_set_priority",toluaI_voice_set_priority0);
 tolua_function(NULL,"voice_check",toluaI_voice_check0);
 tolua_function(NULL,"voice_set_playmode",toluaI_voice_set_playmode0);
 tolua_function(NULL,"voice_get_position",toluaI_voice_get_position0);
 tolua_function(NULL,"voice_set_position",toluaI_voice_set_position0);
 tolua_function(NULL,"voice_get_volume",toluaI_voice_get_volume0);
 tolua_function(NULL,"voice_set_volume",toluaI_voice_set_volume0);
 tolua_function(NULL,"voice_ramp_volume",toluaI_voice_ramp_volume0);
 tolua_function(NULL,"voice_stop_volumeramp",toluaI_voice_stop_volumeramp0);
 tolua_function(NULL,"voice_get_frequency",toluaI_voice_get_frequency0);
 tolua_function(NULL,"voice_set_frequency",toluaI_voice_set_frequency0);
 tolua_function(NULL,"voice_sweep_frequency",toluaI_voice_sweep_frequency0);
 tolua_function(NULL,"voice_stop_frequency_sweep",toluaI_voice_stop_frequency_sweep0);
 tolua_function(NULL,"voice_get_pan",toluaI_voice_get_pan0);
 tolua_function(NULL,"voice_set_pan",toluaI_voice_set_pan0);
 tolua_function(NULL,"voice_sweep_pan",toluaI_voice_sweep_pan0);
 tolua_function(NULL,"voice_stop_pan_sweep",toluaI_voice_stop_pan_sweep0);
 tolua_function(NULL,"voice_set_echo",toluaI_voice_set_echo0);
 tolua_function(NULL,"voice_set_tremolo",toluaI_voice_set_tremolo0);
 tolua_function(NULL,"voice_set_vibrato",toluaI_voice_set_vibrato0);
 tolua_function(NULL,"load_midi",toluaI_load_midi0);
 tolua_function(NULL,"destroy_midi",toluaI_destroy_midi0);
 tolua_function(NULL,"play_midi",toluaI_play_midi0);
 tolua_function(NULL,"play_looped_midi",toluaI_play_looped_midi0);
 tolua_function(NULL,"stop_midi",toluaI_stop_midi0);
 tolua_function(NULL,"midi_pause",toluaI_midi_pause0);
 tolua_function(NULL,"midi_resume",toluaI_midi_resume0);
 tolua_function(NULL,"midi_seek",toluaI_midi_seek0);
 tolua_function(NULL,"midi_out",toluaI_midi_out0);
 tolua_function(NULL,"load_midi_patches",toluaI_load_midi_patches0);
 tolua_function(NULL,"play_audio_stream",toluaI_play_audio_stream0);
 tolua_function(NULL,"stop_audio_stream",toluaI_stop_audio_stream0);
 tolua_function(NULL,"get_audio_stream_buffer",toluaI_get_audio_stream_buffer0);
 tolua_function(NULL,"free_audio_stream_buffer",toluaI_free_audio_stream_buffer0);
 tolua_function(NULL,"get_filename",toluaI_get_filename0);
 tolua_function(NULL,"get_extension",toluaI_get_extension0);
 tolua_function(NULL,"put_backslash",toluaI_put_backslash0);
 tolua_function(NULL,"file_exists",toluaI_file_exists0);
 tolua_function(NULL,"exists",toluaI_exists0);
 tolua_function(NULL,"file_size",toluaI_file_size0);
 tolua_function(NULL,"file_time",toluaI_file_time0);
 tolua_function(NULL,"packfile_password",toluaI_packfile_password0);
 tolua_function(NULL,"pack_fopen",toluaI_pack_fopen0);
 tolua_function(NULL,"pack_fclose",toluaI_pack_fclose0);
 tolua_function(NULL,"pack_fseek",toluaI_pack_fseek0);
 tolua_function(NULL,"pack_fopen_chunk",toluaI_pack_fopen_chunk0);
 tolua_function(NULL,"pack_fclose_chunk",toluaI_pack_fclose_chunk0);
 tolua_function(NULL,"pack_igetw",toluaI_pack_igetw0);
 tolua_function(NULL,"pack_igetl",toluaI_pack_igetl0);
 tolua_function(NULL,"pack_iputw",toluaI_pack_iputw0);
 tolua_function(NULL,"pack_iputl",toluaI_pack_iputl0);
 tolua_function(NULL,"pack_mgetw",toluaI_pack_mgetw0);
 tolua_function(NULL,"pack_mgetl",toluaI_pack_mgetl0);
 tolua_function(NULL,"pack_mputw",toluaI_pack_mputw0);
 tolua_function(NULL,"pack_mputl",toluaI_pack_mputl0);
 tolua_function(NULL,"pack_fread",toluaI_pack_fread0);
 tolua_function(NULL,"pack_fwrite",toluaI_pack_fwrite0);
 tolua_function(NULL,"pack_fgets",toluaI_pack_fgets0);
 tolua_function(NULL,"pack_fputs",toluaI_pack_fputs0);
 tolua_function(NULL,"load_datafile",toluaI_load_datafile0);
 tolua_function(NULL,"unload_datafile",toluaI_unload_datafile0);
 tolua_function(NULL,"load_datafile_object",toluaI_load_datafile_object0);
 tolua_function(NULL,"unload_datafile_object",toluaI_unload_datafile_object0);
 tolua_function(NULL,"get_datafile_property",toluaI_get_datafile_property0);
 tolua_function(NULL,"fixup_datafile",toluaI_fixup_datafile0);
 tolua_function(NULL,"dat",toluaI_dat0);
 tolua_function(NULL,"load_bitmap",toluaI_load_bitmap0);
 tolua_function(NULL,"load_bmp",toluaI_load_bmp0);
 tolua_function(NULL,"load_lbm",toluaI_load_lbm0);
 tolua_function(NULL,"load_pcx",toluaI_load_pcx0);
 tolua_function(NULL,"load_tga",toluaI_load_tga0);
 tolua_function(NULL,"save_bitmap",toluaI_save_bitmap0);
 tolua_function(NULL,"save_bmp",toluaI_save_bmp0);
 tolua_function(NULL,"save_pcx",toluaI_save_pcx0);
 tolua_function(NULL,"save_tga",toluaI_save_tga0);
 tolua_function(NULL,"itofix",toluaI_itofix0);
 tolua_function(NULL,"fixtoi",toluaI_fixtoi0);
 tolua_function(NULL,"ftofix",toluaI_ftofix0);
 tolua_function(NULL,"fixtof",toluaI_fixtof0);
 tolua_function(NULL,"fsqrt",toluaI_fsqrt0);
 tolua_function(NULL,"fatan",toluaI_fatan0);
 tolua_function(NULL,"fatan2",toluaI_fatan20);
 tolua_function(NULL,"fcos",toluaI_fcos0);
 tolua_function(NULL,"fsin",toluaI_fsin0);
 tolua_function(NULL,"ftan",toluaI_ftan0);
 tolua_function(NULL,"facos",toluaI_facos0);
 tolua_function(NULL,"fasin",toluaI_fasin0);
 tolua_function(NULL,"fadd",toluaI_fadd0);
 tolua_function(NULL,"fsub",toluaI_fsub0);
 tolua_function(NULL,"fmul",toluaI_fmul0);
 tolua_function(NULL,"fdiv",toluaI_fdiv0);
 tolua_function(NULL,"alert",toluaI_alert0);
 tolua_function(NULL,"alert3",toluaI_alert30);
 tolua_function(NULL,"file_select",toluaI_file_select0);
 return 1;
}
