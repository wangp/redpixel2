#include <lua.h>
#include <tolua.h>
#include <allegro.h>

int tolua_allegro_open (void);

int main()
{
    lua_open();
    tolua_allegro_open();
    
    allegro_init();
    install_keyboard();
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    install_timer();
    
    set_color_depth(16);
    set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0);
    
    lua_dofile("whee.lua");
    
    return 0;
}
