/*  Benchmarking program for PPCol 
 *  by Peter Wang (tjaden@alphalink.com.au)
 *  Date: 13 Mar 1998 
 */

#include <stdio.h>
#include <allegro.h>
#include <ppcol.h>

#include "../sprites.h"

#define DRAWSTUFF   //comment out this for substantially more accurate results

DATAFILE    *spr;
PPCOL_MASK  *mask[6];
BITMAP      *buffer;
int         draw = 1;
int         white;

volatile long tm = 0;

void my_timer()
{
 tm++;
 }
END_OF_FUNCTION(my_timer)


void benchmark()
{
 int s1, s2;
 int x1, x2;
 int y1, y2;
 int dx, dy;
 int add = 0;
 int dummy = 0;

 /* bounding box collision detection */
 clear_keybuf();
 tm = 4;
 while (!keypressed()) {
   if (tm > 3) {
	 clear(screen);
	 textout(screen, font, "Timing check_bb_collision()", 0, 0, white);
	 if (add) {
       textprintf(screen, font, 0, 10, white, "%d (%d per second).", add, add/4);
       textprintf(screen, font, 0, 18, white, "%d%% collisions.", (dummy*100)/add);
       }
     tm = 0;
     add = 0;
     dummy=0;
     }
   s1 = random()%6;
   s2 = random()%6;
   x1 = 100 + random()%58;
   y1 = 50 + random()%58;
   x2 = 100 + random()%58;
   y2 = 50 + random()%58;
#ifdef DRAWSTUFF
   clear(buffer);
   draw_sprite(buffer, spr[SPRITE_0+s1].dat, x1, y1);
   draw_sprite(buffer, spr[SPRITE_0+s2].dat, x2, y2);
   blit(buffer, screen, 100, 50, 100, 50, 58+56, 58+67);
#endif
   if(check_bb_collision(mask[s1], mask[s2], x1, y1, x2, y2) ) dummy++;
   add++;
   }

 /* pixel perfect collision detection */
 clear_keybuf();
 tm = 4; add = 0; dummy=0;
 while (!keypressed()) {
   if (tm > 3) {
	 clear(screen);
	 textout(screen, font, "Timing check_pp_collision()", 0, 0, white);
	 if (add) {
       textprintf(screen, font, 0, 10, white, "%d (%d per second)", add, add/4);
       textprintf(screen, font, 0, 18, white, "%d%% collisions.", (dummy*100)/add);
       }
	 tm = 0;
	 add = 0;
     dummy=0;
	 }
   s1 = random()%6;
   s2 = random()%6;
   x1 = 100 + random()%58;
   y1 = 50 + random()%58;
   x2 = 100 + random()%58;
   y2 = 50 + random()%58;
#ifdef DRAWSTUFF
   clear(buffer);
   draw_sprite(buffer, spr[SPRITE_0+s1].dat, x1, y1);
   draw_sprite(buffer, spr[SPRITE_0+s2].dat, x2, y2);
   blit(buffer, screen, 100, 50, 100, 50, 58+56, 58+67);
#endif
   if(check_pp_collision(spr[SPRITE_0+s1].dat, spr[SPRITE_0+s2].dat, x1, y1, x2, y2)) dummy++;
   add++;
   }

 /* bit masked pixel perfect collision detection */
 clear_keybuf();
 tm = 4; add = 0; dummy=0;
 while (!keypressed()) {
   if (tm > 3) {
	 clear(screen);
     textout(screen, font, "Timing check_ppmask_collision()", 0, 0, white);
     if (add) {
       textprintf(screen, font, 0, 10, white, "%d (%d per second)", add, add/4);
       textprintf(screen, font, 0, 18, white, "%d%% collisions.", (dummy*100)/add);
       }
     tm = 0;
	 add = 0;
     dummy=0;
	 }
   s1 = random()%6;
   s2 = random()%6;
   x1 = 100 + random()%58;
   y1 = 50 + random()%58;
   x2 = 100 + random()%58;
   y2 = 50 + random()%58;
#ifdef DRAWSTUFF
   clear(buffer);
   draw_sprite(buffer, spr[SPRITE_0+s1].dat, x1, y1);
   draw_sprite(buffer, spr[SPRITE_0+s2].dat, x2, y2);
   blit(buffer, screen, 100, 50, 100, 50, 58+56, 58+67);
#endif
   if(check_ppmask_collision(mask[s1], mask[s2], x1, y1, x2, y2)) dummy++;
   add++;
   }

 /* Do pixel perfect collision amount detection */
 clear_keybuf();
 tm = 4; add = 0; dummy=0;
 while (!keypressed()) {
   if (tm > 3) {
	 clear(screen);
	 textout(screen, font, "Timing check_pp_collision_amount()", 0, 0, white);
     if (add) {
       textprintf(screen, font, 0, 10, white, "%d (%d per second)", add, add/4);
       textprintf(screen, font, 0, 18, white, "Average amount: %d.", dummy/add);
       }
	 tm = 0;
     add = 0;
     dummy=0;
	 }
   s1 = random()%6;
   s2 = random()%6;
   x1 = 100 + random()%58;
   y1 = 50 + random()%58;
   x2 = 100 + random()%58;
   y2 = 50 + random()%58;
   do {
     dx = random()%3-1;
     dy = random()%3-1;
     } while ((dx==0) && (dy==0));
#ifdef DRAWSTUFF
   clear(buffer);
   draw_sprite(buffer, spr[SPRITE_0+s1].dat, x1, y1);
   draw_sprite(buffer, spr[SPRITE_0+s2].dat, x2, y2);
   blit(buffer, screen, 100, 50, 100, 50, 58+56, 58+67);
#endif
   dummy+=check_pp_collision_amount(spr[SPRITE_0+s1].dat, spr[SPRITE_0+s2].dat, x1, y1, x2, y2,dx,dy);
   add++;
   }

 /* Do bitmasked pixel perfect collision amount detection */
 clear_keybuf();
 tm = 4; add = 0; dummy=0;
 while (!keypressed()) {
   if (tm > 3) {
	 clear(screen);
     textout(screen, font, "Timing check_ppmask_collision_amount()", 0, 0, white);
     if (add) {
       textprintf(screen, font, 0, 10, white, "%d (%d per second)", add, add/4);
       textprintf(screen, font, 0, 18, white, "Average amount: %d.", dummy/add);
       }
	 tm = 0;
	 add = 0;
     dummy=0;
     }
   s1 = random()%6;
   s2 = random()%6;
   x1 = 100 + random()%58;
   y1 = 50 + random()%58;
   x2 = 100 + random()%58;
   y2 = 50 + random()%58;
   do {
     dx = random()%3-1;
     dy = random()%3-1;
     } while ((dx==0) && (dy==0));
#ifdef DRAWSTUFF
   clear(buffer);
   draw_sprite(buffer, spr[SPRITE_0+s1].dat, x1, y1);
   draw_sprite(buffer, spr[SPRITE_0+s2].dat, x2, y2);
   blit(buffer, screen, 100, 50, 100, 50, 58+56, 58+67);
#endif
   dummy+=check_ppmask_collision_amount(mask[s1], mask[s2], x1, y1, x2, y2,dx,dy);
   add++;
   }

 }



int main()
{
    int card    = GFX_AUTODETECT,
	width   = 320,
	height  = 200,
	depth   = 8;
    int i, k;

    allegro_init();
    install_keyboard();
    install_mouse();
    install_timer(); 

    LOCK_FUNCTION(my_timer);
    LOCK_VARIABLE(tm);
    install_int_ex(my_timer, SECS_TO_TIMER(1));

    set_gfx_mode(GFX_VGA,320,200,0,0);
    set_palette(desktop_palette);
    if (!gfx_mode_select_ex(&card, &width, &height, &depth))
	return 1;

    set_color_depth(depth);
    if(set_gfx_mode(card, width, height, 0, 0)) {printf(allegro_error); return 1;}

    spr = load_datafile("../sprites.dat");
    if (!spr) 
    {
	set_gfx_mode(GFX_TEXT,0,0,0,0);
	printf("Couldn't load sprites.dat");
	return 1;
    } 

    for(i=0; i<6; i++)
	mask[i] = create_ppcol_mask(spr[SPRITE_0+i].dat);

    if (depth != 8)
    {
	/* sorry about this... 2am hack */
        select_palette(spr[PAL].dat);
	unload_datafile(spr);
        spr = load_datafile("../sprites.dat");

	/* convert (0,0,0) -> (255,0,255) */
        for (i=SPRITE_0; i<SPRITE_0+6; i++)
	{
	    int u, v;
	    for (v=0; v<((BITMAP *)spr[i].dat)->h; v++)
		for (u=0; u<((BITMAP *)spr[i].dat)->w; u++)
		    if (getpixel(spr[i].dat, u, v)==makecol(0,0,0))
			putpixel(spr[i].dat, u, v, makecol(255,0,255));
	}
    }
    else
	set_palette(spr[PAL].dat);

    buffer = create_bitmap(320, 200);   // NOTE: 320x200 is on purpose
    white = makecol(255,255,255);

    for (;;)
    {
	clear(buffer);
	textout(buffer, font, "PPCol benchmarking program", 0, 0, white);
	textout(buffer, font, "Press space to start  ESC quits", 0, 10, white);
	blit(buffer, screen, 0, 0, 0, 0, 320, 200); 

	while (!keypressed());
	k = readkey() >> 8;
	if (k==KEY_SPACE) {
	    benchmark();
	    clear_keybuf();
	}
	else if (k==KEY_ESC)
	    break; 
    }

    for (i=0; i<6; i++)
	destroy_ppcol_mask(mask[i]);
    unload_datafile(spr);

    #ifdef DRAWSTUFF
    set_gfx_mode(GFX_TEXT,0,0,0,0);
    printf("If you want accurate results (or fair results?), edit the BMARK.C file\nand remove the #define DRAWSTUFF statement at the beginning.\n");
    #endif

    return 0;
}

END_OF_MAIN ();
