/* export.c
 * 
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <allegro.h>
#include "stone.h"
#include "export.h"


void export_symbols ()
{
#define e(NAME) stone_export (#NAME, (void *) &NAME)

    /* libc */
    
    e (__bzero);
    e (calloc);
    e (free);
    e (malloc);
    e (memcpy);
    e (memset);
    e (realloc);

    e (__strdup);
    e (printf);
    e (snprintf);
    e (strcat);
    e (strcmp);
    e (strcpy);
    e (strdup);
    e (strlen);
    e (vsnprintf);

    e (atexit);
    e (qsort);    
    e (sleep);
    
    /* allegro */

    e (system_driver);

    e (ugetc);
    e (ugetx);
    e (usetc);
    e (uwidth);
    e (ucwidth);
    e (uisok);
    e (uoffset);
    e (ugetat);
    e (usetat);
    e (uinsert);
    e (uremove);
    e (utolower);
    e (utoupper);
    e (uisspace);
    e (uisdigit);
    e (ustrsize);
    e (ustrsizez);
    e (ustrcpy);
    e (ustrcat);
    e (ustrlen);
    e (ustrcmp);
    e (ustrncpy);
    e (ustrncat);
    e (ustrncmp);
    e (ustricmp);
    e (ustrlwr);
    e (ustrupr);
    e (ustrchr);
    e (ustrrchr);
    e (ustrstr);
    e (ustrpbrk);
    e (ustrtok);
    e (uatof);
    e (ustrtol);
    e (ustrtod);
    e (ustrerror);
    e (uvsprintf);
    e (usprintf);
    e (_ustrdup);
    
    e (mouse_x);
    e (mouse_y);
    e (mouse_z);
    e (mouse_b);
    e (mouse_pos);
    e (show_mouse);
    e (scare_mouse);
    e (unscare_mouse);
    e (position_mouse);
    e (position_mouse_z);
    e (set_mouse_sprite);
    e (set_mouse_sprite_focus);

    e (install_int_ex);
    e (install_int);
    e (remove_int);
    e (rest);
    
    e (keyboard_lowlevel_callback);
    e (key);
    e (key_shifts);
    e (keypressed);
    e (readkey);
    e (ureadkey);
    e (clear_keybuf);

    e (gfx_driver);
    e (screen);
    e (create_bitmap);
    e (create_bitmap_ex);
    e (create_sub_bitmap);
    e (create_video_bitmap);
    e (create_system_bitmap);
    e (destroy_bitmap);
    
    e (makecol);
    
    e (set_clip);
    e (rect);
    e (circle);
    e (circlefill);
    e (blit);
    e (masked_blit);
    e (stretch_blit);
    e (masked_stretch_blit);
    e (rotate_sprite);
    e (rotate_scaled_sprite);
    e (pivot_sprite);
    e (pivot_scaled_sprite);
    e (clear);
    
    e (font);
    e (text_mode);
    e (textout);
    e (textout_centre);
    e (textout_right);
    e (textout_justify);
    e (textprintf);
    e (textprintf_centre);
    e (textprintf_right);
    e (textprintf_justify);
    e (text_length);
    e (text_height);

    e (create_sample);
    e (destroy_sample);
    e (play_sample);
    e (stop_sample);

    e (fix_filename_path);
    e (replace_filename);
    e (replace_extension);
    e (append_filename);
    e (get_filename);
    e (get_extension);
    e (file_exists);
    e (exists);
    e (for_each_file);
    e (pack_fopen);
    e (pack_fclose);
    e (pack_fseek);
    e (pack_igetw);
    e (pack_igetl);
    e (pack_iputw);
    e (pack_iputl);    
    e (pack_mgetw);
    e (pack_mgetl);
    e (pack_mputw);
    e (pack_mputl);
    e (pack_fread);
    e (pack_fwrite);
    e (pack_fgets);
    e (pack_fputs);
    
    e (load_datafile);
    e (unload_datafile);
    e (get_datafile_property);
    e (load_bitmap);
    e (save_bitmap);

    e (fsqrt);
    e (fhypot);
    e (fatan); 
    e (fatan2);
    e (_cos_tbl);
    e (_tan_tbl);
    e (_acos_tbl);
        
#undef e
}
