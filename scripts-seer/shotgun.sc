/* scripts/rpg.sc	-*- mode: C -*- */

#author "Psyk"
#title "shotgun.sc"

#include "scripts/allegro.sh"
#include "scripts/imports.sh"

export script_main;
export script_exit;


DATAFILE *dat;
BITMAP *ammo, *weapon, *status;
int typenum;


void ammo_pickup(msg_t msg, object_t *obj, player_t *pl)
{
    /*
    switch (msg)
    {
      case MSG_TOUCHED:
	p->ammo[number] += 5;			
	destroy_item(i);
	break;
    }*/	
}


void weapon_pickup(msg_t msg, object_t *obj, player_t *pl)
{
    /*
    switch (msg)
    {
      case MSG_TOUCHED:
	p->have_weapon[number] = 1;
	destroy_item(i);
	break;
    }
     */
}


void shotgun(msg_t msg, player_t *pl)
{
    /*
    switch (msg)
    {
      case MSG_DRAW:
	draw_sprite(dbuf, status, STATUS_X, STATUS_Y);
	break;
      case MSG_FIRE:
	draw_sprite ( .... );
    }*/
}


/* script_main:
 *  Initialise the pickup, the weapon, the ammo pickup and the projectiles.
 *  Return zero on success, non-zero on error.
 */
int script_main()
{
    dat = load_datafile("data/shotgun.dat");
    if (!dat)
      return -1;

    ammo 	= df_dat(df_get_item(dat, "ammo"));
    weapon 	= df_dat(df_get_item(dat, "weapon"));
    status 	= df_dat(df_get_item(dat, "status"));

    typenum = create_weapon_type("shotgun");
    assign_weapon_handler(typenum, addressof("shotgun"));
    
    create_object_type("shotgun;ammo", addressof("ammo_pickup"), ammo);
    create_object_type("shotgun;weapon", addressof("weapon_pickup"), weapon);
       
    return 0;
}


/* script_exit:
 *  Automagically called at  the end of the program.
 *  Free any memory in local variables.
 *  Weapons and items will be deleted before this is called.
 */
void script_exit()
{
    unload_datafile(dat);
}

