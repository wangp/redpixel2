/* engine.c:
 *  The game engine - very ugly hack. (test only)
 */

#include <allegro.h>
#include <seer.h>

#include "common.h"
#include "export.h"
#include "player.h"
#include "report.h"
#include "rpx.h"
#include "script.h"
#include "tiles.h"
#include "wrapper.h"



#define LOGFILE		"engine-seer-debug.log"
#define TEST_MAP 	"whee.rpx"


DATAFILE *playerdat;
DATAFILE *deathseq;

int local_player = 0;

int mx, my;

int double_size = 0;



/* ----------------------------- timer stuff --------------------------*/

#define GAME_SPEED	40		   /* in fps */

volatile int speed_counter = 0;

void speed_timer()
{
  speed_counter++;
} END_OF_FUNCTION(speed_timer)



/* ----------------------------- corpse stuff --------------------------*/
      
#define MAX_CORPSES	64      
      
struct seq {
    char *start;
    int numframes;
} 
seq[] = 
{
    { "000", 17 },
    { "a000", 16 },
    { "b000", 18 },
    { "c000", 17 },
    { "d000", 21 },
    { NULL, 0 }
};
      
typedef struct corpse
{
    int x, y;
    int startframe, lastframe, framenum;
    int tics;
    int alive;			       /* hehe */
} corpse_t;

corpse_t corpse[MAX_CORPSES];

void create_corpse(int x, int y)
{
    int i, r;
    
    r = random() % 5;
    
    for (i=0; i<MAX_CORPSES; i++)
    {
	if (!corpse[i].alive)
	{
	    corpse[i].alive = 1;
	    corpse[i].x = x;
	    corpse[i].y = y;
	    corpse[i].framenum = corpse[i].startframe = df_get_item_num(deathseq, seq[r].start);
	    corpse[i].lastframe = corpse[i].startframe + seq[r].numframes - 1;
	    corpse[i].tics = 0;
	    //printf("created, start frame = %d!\n", corpse[i].startframe);
	    return;
	}
    }
}

void animate_corpses()
{
    int i;
    
    for (i=0; i<MAX_CORPSES; i++)
    {
	if (corpse[i].alive && corpse[i].framenum < corpse[i].lastframe)
	{
	    if (++corpse[i].tics > 2) {
		corpse[i].framenum++;
		corpse[i].tics = 0;
	    }
	}
    }
}

void draw_corpses(BITMAP *dest)
{
    int i;
    
    for (i=0; i<MAX_CORPSES; i++)
    {
	if (corpse[i].alive)
	{
	    draw_sprite(dbuf, deathseq[corpse[i].framenum].dat, corpse[i].x - mx, corpse[i].y - my);
	}
    }
}



/* ----------------------------- other stuff --------------------------- */




/* draw_tiles:
 *  Draw tiles, beginning at spec. offset (in pixels)
 */

void draw_tiles(BITMAP *dest, int offx, int offy) 
{
    tiledata_t *td;
    int ubeg, vbeg, uend, vend, u, v;
    int x, y, t;
    
    ubeg = offx / TILE_W;
    vbeg = offy / TILE_H;
    uend = ubeg + SCREEN_W / TILE_W + 1;
    vend = vbeg + SCREEN_H / TILE_H + 2;
    
    //printf("ubeg: %d, uend %d, vbeg %d, vend %d\n", ubeg, uend, vbeg, vend);
    
    clear(dest);
    
    y = - (offy % TILE_H);
    for (v = vbeg; v < vend; v++)
    {
	x = -(offx % TILE_W);
	for (u = ubeg; u < uend; u++)
	{
	    t = rpx.tile[v][u];
	    if (t != TILE_BLANK) {
		td = tiles->tbl[t].data;
		draw_sprite(dest, td->bmp, x, y);
	    }
	    
	    //printf("%d\n", t);
	    
	    x += TILE_W;
	}
	
	y += TILE_H;
    }    
}



static int _player_tile_col(PPCOL_MASK *plmask, int plx, int ply, int tx, int ty)
{
    int t;
    tiledata_t *td;
    PPCOL_MASK *tmask;

    t = rpx.tile[ty][tx];
    if (t == TILE_BLANK) 
      return 0;
    
    td = tiles->tbl[t].data;
    tmask = td->mask;
    return check_ppmask_collision(plmask, tmask, plx, ply, tx*TILE_W, ty*TILE_H);
}

int check_player_tile_collision(BITMAP *pl, int x, int y)
{    
    int tx, ty;
    PPCOL_MASK *plmask;
    
    tx = x / TILE_W;
    ty = y / TILE_H;
    
    plmask = create_ppcol_mask(pl);
    
    /* we assume player is not bigger than a tile */
    if (_player_tile_col(plmask, x, y, tx, ty)) goto hit;
    if (_player_tile_col(plmask, x, y, tx+1, ty)) goto hit;
    if (_player_tile_col(plmask, x, y, tx, ty+1)) goto hit;
    if (_player_tile_col(plmask, x, y, tx+1, ty+1)) goto hit;
    
    destroy_ppcol_mask(plmask);
    return 0;
    
    hit:			       /* NNNOOooo... -  A GOTO!  */
    destroy_ppcol_mask(plmask);
    return -1;
}


void move_player_legs(player_t *pl)
{
    if (++pl->legtics > 2)
    {
	pl->legtics = 0;
	if (++pl->legframe > 7)
	  pl->legframe = 0;
    }
}


void build_player(player_t *pl)
{
    clear_to_color(pl->bmp, makecol(255,0,255));   /* only for 16bit */
    draw_sprite(pl->bmp, playerdat[0].dat, 0, 0);
    draw_sprite(pl->bmp, playerdat[df_get_item_num(playerdat, "walkie_000") + pl->legframe].dat, 0, 0);
    //printf("here!\n");
}


	
/* quality_seconds:
 *  I think this describes the game quite well (yeah right)
 * THIS WHOLE ROUTINE IS FIXME!
 */

void quality_seconds()
{
    int offsetx, offsety;
    int update = 1;
    player_t *pl = &player[local_player];
          
    while (!key[KEY_Q])
    {
	while (speed_counter > 0)
	{
	    if (key[KEY_A]) pl->x--;
	    //if (key[KEY_S]) pl->y++;
	    if (key[KEY_D]) pl->x++;
	    if (key[KEY_W]) {
		if (pl->vy == 0)
		  pl->vy = -10;
	    }
	    if (pl->vy < 0)
	      pl->vy++;
	    if (key[KEY_SPACE]) {
		create_corpse(pl->x, pl->y);
		while (key[KEY_SPACE]);
	    }
	    
	    move_player_legs(pl);
	    
	    pl->y++;
	    if (check_player_tile_collision(pl->bmp, pl->x, pl->y)) {   /* crap gravity */
		pl->y--;
	    }
	    
	    pl->y += pl->vy;
	    pl->x += pl->vx;
	    
	    animate_corpses();
	    
	    update = 1;
	    speed_counter--;
	}
	   	
	if (update)
	{
	    /* this should average between pl and mouse pos */
	    
	    offsetx = pl->x - SCREEN_W/2;
	    offsety = pl->y - SCREEN_H/2;
	
	    if (offsetx < 0) offsetx = 0;
	    if (offsety < 0) offsety = 0;
	    /* ... */
	    
	    mx = offsetx;
	    my = offsety;
	    
	    draw_tiles(dbuf, offsetx, offsety);
	    draw_corpses(dbuf);
	    build_player(pl);
	    draw_sprite(dbuf, pl->bmp, pl->x - mx, pl->y - my);
	    if (!double_size)
	      blit(dbuf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	    else
	      stretch_blit(dbuf, screen, 0, 0, SCREEN_W/2, SCREEN_H/2, 0, 0, SCREEN_W, SCREEN_H);
	    
	    update = 0;
	}
    }
}



/* main: 
 *  FIXME: Temp.
 */
int main(int argc, char **argv)
{
    int w = 320, h = 200, bpp = 16;

    scOpen_Debug(LOGFILE);
    scToggle_Debug(true);                                

    allegro_init();
    install_keyboard();
    install_timer();
    if (install_mouse() == -1) {
	consoleprintf("Error calling install_mouse.  Perhaps you need to install a mouse driver?\n");
	return 1;
    }

    LOCK_VARIABLE(speed_counter);
    LOCK_FUNCTION(speed_timer);
    install_int_ex(speed_timer, BPS_TO_TIMER(GAME_SPEED));

    set_color_depth(bpp);
    
    if (double_size) {
	w *= 2;
	h *= 2;
    }
    
    if (set_gfx_mode(GFX_AUTODETECT, w, h, 0, 0) < 0) {
	consoleprintf("Couldn't set video mode %dx%dx%dbpp\n", w, h, bpp);
	return 1;
    }
    
    playerdat = load_datafile("data/player.dat");
    deathseq = load_datafile("data/die.dat");

    dbuf = create_bitmap(SCREEN_W, SCREEN_H);

    create_tiles_table();
    
    export();
    exec_script("scripts/init.sc", "init");
    
    load_rpx(TEST_MAP);
    
    player[local_player].x = player[local_player].y = 
      player[local_player].vx = player[local_player].vy = 0;
    player[local_player].bmp = create_bitmap(16, 32);
        
    quality_seconds();
    
    destroy_tiles_table();
    
    destroy_bitmap(dbuf);
    
    unload_datafile(playerdat);
    unload_datafile(deathseq);
    
    destroy_bitmap(player[local_player].bmp);
    
    free_scripts();
    scClose_Debug();            
    
    return 0;
}
