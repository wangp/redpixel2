/*****************************************************************************
 PPCol Library example program by Grzegorz Adam Hankiewicz.
 Modified a little by Ivan Baldo: changed and added some comments, made it
 allocate masks only for sprites and not for all the objects in the datafile,
 and added more checks for errors in the reserve_dynamic_mask function.
 Based in the ex1.c example program of the PPCol Library, the difference is
 that this program uses a cool method to dinamically and automatically create
 and store the masks of a datafile and can be usefull for someone.
 Quoted from Grzegorz:
 "This shows how to allocate dynamically the masks, instead of having to
 declare a fixed array of them. This may be great if the number of graphics
 in your datafiles changes or you want to create a generic routine which
 accepts every number of sprites/bitmasks."
*****************************************************************************/

#include <stdio.h>
#include <allegro.h>
#include <ppcol.h>
#include "../sprites.h" //Datafile generated header containing some sprite index and a pallete. 

typedef struct DYNAMIC_MASK { //DYNAMIC_MASK is the name of the structure.
	PPCOL_MASK **p; //This is an array of pointers to pointers of type PPCOL_MASK...
   int num; //and this is the number of p elements (number of masks).
   } DYNAMIC_MASK; //DYNAMIC_MASK is the name of the typedef.

DATAFILE *data;
BITMAP *sbuffer; //This will be the screen buffer on memory...

int X1, Y1; //Positions of the sprites.
int X2, Y2;
int SP1, SP2; //This stores what sprite we are using from the datafile...
int MASK; //This tells if we have to show the mask or not...

DYNAMIC_MASK *sprite_masks; //Array of pointers to sprite masks but with the structure of the DYNAMIC_MASK.

int create_sprite_masks(); //This function will initialize all the masks for all the sprites. You have to look at this function! If there is an error, it returns 1.
void destroy_sprite_masks(); //This function will deinitialize all the masks of all the sprites. See it.
void move_sprites(); //This function will check for the correct keys and move the sprite.
void draw_sprites(); //This function will clear the buffer to black and draw the sprites in their respective positions...
void show_info(); //This function will show the positions of the sprites and will show if there is bounding box collision or pixel perfect collision using masks and direct bitmap data access method. This is the function where you have to look!
void blit_screen(); //This function will blit the buffer to the screen.
void free_dynamic_mask (DYNAMIC_MASK *d); //Frees the allocated masks.
DYNAMIC_MASK *reserve_dynamic_mask(DATAFILE *dat); //Pass a datafile and it will create all masks for it and returns a pointer to a DYNAMIC_MASK structure type.


int main(void)
{
 int card=GFX_AUTODETECT;
 int width=320;
 int height=240;
 int depth=8;

 allegro_init();
 install_timer(); //For vsync() and the video mode selection screen...
 install_keyboard();
 install_mouse(); //For the video mode selection menu...

 set_gfx_mode(GFX_VGA,320,200,0,0);
 set_palette(desktop_palette);
 if (!gfx_mode_select_ex(&card, &width, &height, &depth)) return 1; //The user has pressed ESC to quit the example before selecting a video mode...

 set_color_depth(depth); //First set the requested color depth...
 if(set_gfx_mode(card, width, height, 0, 0)) {printf(allegro_error); return 2;} //There was a problem setting the requested video mode... show Allegro error information...

 clear(screen);

 //Initialize the variables...
 X1=width/3; Y1=height/3;
 X2=X1*2; Y2=Y1*2;
 SP1=1; SP2=2;
 MASK=0; //Don't show the masks...

 data=load_datafile("../sprites.dat");
 if(!data) {
   set_gfx_mode(GFX_TEXT,0,0,0,0);
   printf("Couldn't load sprites.dat!");
   return 1;
   }

 if(depth!=8) { //Transparent color conversion method by Peter Wang.
   int SPRNUM, X,Y;
   select_palette(data[PAL].dat);
   unload_datafile(data); //This and the next is because we have loaded the datafile with a wrong palette thus the conversion between 8 colors and Xbpp format fails.
   data=load_datafile("../sprites.dat");
   for(SPRNUM=SPRITE_0; SPRNUM<SPRITE_0+6; SPRNUM++) {
      for(Y=0; Y< ((BITMAP *)data[SPRNUM].dat)->h; Y++) {
         for(X=0; X< ((BITMAP *)data[SPRNUM].dat)->w; X++) {
            if( getpixel(data[SPRNUM].dat, X,Y)==makecol(0,0,0) ) putpixel(data[SPRNUM].dat, X,Y, makecol(255,0,255) );
            }
         }
      }
   } else {
   set_palette(data[PAL].dat);
   }

 sbuffer=create_bitmap(width,height);

 if(create_sprite_masks()) return 1; //We call this function to initialize the masks for all the sprites. This is a function used in this example, it is not a PPCol library function! It returns 1 if there is an error, 0 if all is correct.

 while(!key[KEY_ESC]) { //While the user is not bored...
   move_sprites();
   draw_sprites();
   show_info();
   blit_screen();
   vsync(); //This prevents the movement going too fast.
   }

 destroy_sprite_masks();

 return 0;
}

END_OF_MAIN();


//This function will create the masks for every sprite...
int create_sprite_masks()
{
 sprite_masks = reserve_dynamic_mask(data); //Uses our function to dinamically allocate and create the masks for the sprites of the datafile we pass.
 if (sprite_masks == NULL) { //If there was an error creating the masks...
      set_gfx_mode(GFX_TEXT,0,0,0,0);
      printf("Error creating mask for sprites!");
      return 1;
 }

 return 0;
}

//This function will deinitialize all the masks of all the sprites.
void destroy_sprite_masks()
{
 free_dynamic_mask(sprite_masks); //This will free the memory of all the dinamically created masks.
}



//This function will check for some keys and move the sprites and select the sprites...
void move_sprites()
{
 //Move sprite 1 with the arrow keys...
 if(key[KEY_UP]) Y1--; else if(key[KEY_DOWN]) Y1++;
 if(key[KEY_LEFT]) X1--; else if(key[KEY_RIGHT]) X1++;
 //Check if sprite 1 is in range...
 if(Y1<0) Y1=0;
 if(Y1>SCREEN_H) Y1=SCREEN_H;
 if(X1<0) X1=0;
 if(X1>SCREEN_W) X1=SCREEN_W;

  //Move sprite 2 with the keyboard keys...
 if(key[KEY_W]) Y2--; else if(key[KEY_X]) Y2++;
 if(key[KEY_A]) X2--; else if(key[KEY_D]) X2++;
 //Check if sprite 2 is in range...
 if(Y2<0) Y2=0;
 if(Y2>SCREEN_H) Y2=SCREEN_H;
 if(X2<0) X2=0;
 if(X2>SCREEN_W) X2=SCREEN_W;

 //Select sprites with the 1 and 2 keys respectivelly...
 if(key[KEY_1]) { SP1++; clear_keybuf(); } //clear_keybuf() is used to slow down the keypress...
 if(key[KEY_2]) { SP2++; clear_keybuf(); }
 if(SP1>6) SP1=1;
 if(SP2>6) SP2=1;

 //If you press M you can toggle the mask view...
 if(key[KEY_M]) {
   if(MASK) MASK=0; else MASK=1;
   clear_keybuf();
   }

 }


// This function will clear the screen buffer to color black and draw the sprites...
void draw_sprites()
{
 int CHUNK;  //Stores the current chunk for the mask view...
 int ROW;    //Stores the current row for the mask view...
 int CHUNKP; //Stores the current position in the chunk for the mask view...

 clear(sbuffer);

 draw_sprite(sbuffer, data[SP1].dat, X1, Y1);
 draw_sprite(sbuffer, data[SP2].dat, X2, Y2);

 //Draw sprite masks if the user wants...
 if(MASK) {
   for(CHUNK=0; CHUNK<=sprite_masks->p[SP1]->max_chunk; CHUNK++) {
      for(ROW=0; ROW<sprite_masks->p[SP1]->h; ROW++) {
         for(CHUNKP=CHUNK*32; CHUNKP<(CHUNK*32+32); CHUNKP++) {
             if( sprite_masks->p[SP1]->sp_mask[ROW][CHUNK]<<CHUNKP & 0x80000000) putpixel(sbuffer, X1+CHUNKP, Y1+ROW, makecol(64,255,64) );
             }
         }
      }
   for(CHUNK=0; CHUNK<=sprite_masks->p[SP2]->max_chunk; CHUNK++) {
      for(ROW=0; ROW<sprite_masks->p[SP2]->h; ROW++) {
         for(CHUNKP=CHUNK*32; CHUNKP<(CHUNK*32+32); CHUNKP++) {
             if( sprite_masks->p[SP2]->sp_mask[ROW][CHUNK]<<CHUNKP & 0x80000000) putpixel(sbuffer, X2+CHUNKP, Y2+ROW, makecol(64,255,64) );
             }
         }
      }

   }

 }


//This function will show the positions of the sprites and will show if there is bounding box collision or pixel perfect collision using masks and direct bitmap data access method. This is the function where you have to look!
void show_info()
{
 int LINE=0;
 textout(sbuffer,font,"Keys: 1, 2, M, Arrows, A, D, W, X, ESC.",7,LINE*8,makecol(255,255,32) );

 LINE++;

 if( check_bb_collision(sprite_masks->p[SP1], sprite_masks->p[SP2], X1,Y1, X2,Y2) ) { //Check for bounding box collision... you can use the sprites instead of the masks for this... there's no difference!
   textout(sbuffer,font,"Bounding box collision: YES!",7,LINE*8,makecol(255,255,255) );
   } else {
   textout(sbuffer,font,"Bounding box collision: NO. ",7,LINE*8,makecol(191,191,191) );
   }

 LINE++;

 if( check_ppmask_collision(sprite_masks->p[SP1], sprite_masks->p[SP2], X1,Y1, X2,Y2) ) { //Check for pixel perfect collision using the masks.
   textout(sbuffer,font,"Bitmasked pixel perfect collision: YES!",7,LINE*8,makecol(255,255,255) );
   } else {
   textout(sbuffer,font,"Bitmasked pixel perfect collision: NO. ",7,LINE*8,makecol(191,191,191) );
   }

 LINE++;

 if( check_pp_collision(data[SP1].dat, data[SP2].dat, X1,Y1, X2,Y2) ) { //Check for pixel perfect collision accessing directly to the bitmap data.
   textout(sbuffer,font,"Pixel perfect collision: YES!",7,LINE*8,makecol(255,255,255) );
   } else {
   textout(sbuffer,font,"Pixel perfect collision: NO. ",7,LINE*8,makecol(191,191,191) );
   }

 }

//This will draw everything...
void blit_screen()
{
 blit(sbuffer,screen,0,0, 0,0, SCREEN_W,SCREEN_H);
 }


/* Up to this line is the normal ex1.c example just modified to support the
 * dynamical allocation of bitmasks. Here are the routines used to allocate
 * and free the memory for the bitmasks. As you see, DYNAMIC_MASK is a
 * structure that holds the number of bitmasks and a pointer to bitmasks'
 * pointers. The structure itself is also defined as pointer.
 */


//Frees the dinamically allocated masks.
void free_dynamic_mask(DYNAMIC_MASK *d)
{
	int i;

   for (i=0; i < d->num; i++) //Loop through all the masks created...
   	if ( d->p[i] ) //And if the mask was allocated (pointer not NULL)...
	   	destroy_ppcol_mask( d->p[i] ); //Deallocate the mask...

	free(d->p); //Deallocate the pointer to array of masks...
   free(d); //Deallocate the pointer to the DYNAMIC_MASK structure...
}


//Pass a datafile and it will create all masks for it and returns a pointer to a DYNAMIC_MASK structure type.
DYNAMIC_MASK *reserve_dynamic_mask(DATAFILE *dat)
{
	int f,c;
   BITMAP *bmp; //Used to store the pointer to some temporary bitmaps (for the support for COMPILED and RLE sprites)...
	DYNAMIC_MASK *temp; //The pointer to the structure that we will return.

   // First allocate memory for the structure itself...
   temp = malloc ( sizeof(DYNAMIC_MASK));
   if (temp == NULL) return NULL; //If the memory allocation fails, return that we have fail.

   // We want to search the number of sprites in the datafile...
   for (f=0,c=1; dat[f].type != DAT_END; f++) {
   	switch(dat[f].type) {
         //If it is a sprite...
         case DAT_C_SPRITE:
			case DAT_XC_SPRITE:
			case DAT_RLE_SPRITE:
			case DAT_BITMAP:
              //Increase the number of sprites...
              c++; //This statement reminds me of some language... :-))
              break;
         //If it is other object... do nothing!
         default:
              break;
         }
      }

   // Allocate memory for the array of pointers to pointers of type PPCOL_MASK...
   temp->p = malloc ( sizeof(PPCOL_MASK *) * (c)); //c is the number of objects in the datafile...
	if (temp->p == NULL) { //If the allocation failed...
   	free (temp); //Release the memory of the structure...
      return NULL; //And tell that we have failed...
   }

   // This stores the number of masks in the structure...
   temp->num = c;

   // Now we clear the array of pointers to pointers...
   for (f=0; f< temp->num; f++)
   	temp->p[f] = NULL;

	/* And now we assign them. Since the data can be of different types, we
    * need different ways of creating the bitmask. The only thing we do is
    * check the type of the data. If BITMAP, then create the mask, if
    * RLE_SPRITE or COMPILED_SPRITE, create a temporary bmp and then use
    * it to create the mask.
    */
   for (f=0; dat[f].type != DAT_END; f++){
   	switch(dat[f].type) {
			case DAT_C_SPRITE:
			case DAT_XC_SPRITE:
         	bmp = create_bitmap ( ((COMPILED_SPRITE *) dat[f].dat)->w, ((COMPILED_SPRITE *) dat[f].dat)->h); //We create a bitmap with the same size of our sprite.
            if ( bmp == NULL) { //If there is an error with the bitmap creation...
            	temp->num = f; //We change the number of allocated masks to the number of succesfully allocated masks...
               free_dynamic_mask(temp); //and we call our function to deallocate the dynamic masks structure...
            	return NULL; //then we tell that we have failed...
            }

            clear(bmp); //We clear it so it has the transparent color...
            draw_compiled_sprite(bmp, dat[f].dat,0,0); //We draw our sprite on that temporary bitmap...
         	temp->p[f] = create_ppcol_mask(bmp); //Create the mask using the temporary bitmap.
            destroy_bitmap(bmp); //Release our temporary bitmap...

            if ( temp->p[f] == NULL) { //If the mask creation fails...
            	temp->num = f; //We change the number of allocated masks to the number of succesfully allocated masks...
               free_dynamic_mask(temp); //and we call our function to deallocate the dynamic masks structure...
            	return NULL; //then we tell that we have failed...
            }

            break;

			case DAT_RLE_SPRITE:
         	bmp = create_bitmap ( ((RLE_SPRITE *) dat[f].dat)->w, ((RLE_SPRITE *) dat[f].dat)->h); //We create a bitmap with the same size of our sprite.
            if ( bmp == NULL) { //If there is an error with the bitmap creation...
            	temp->num = f; //We change the number of allocated masks to the number of succesfully allocated masks...
               free_dynamic_mask(temp); //and we call our function to deallocate the dynamic masks structure...
            	return NULL; //then we tell that we have failed...
            }

            clear (bmp); //We clear it so it has the transparent color...
            draw_rle_sprite(bmp, dat[f].dat,0,0); //We draw our sprite on that temporary bitmap...
         	temp->p[f] = create_ppcol_mask(bmp); //Create the mask using the temporary bitmap.
            destroy_bitmap(bmp); //Release our temporary bitmap...

            if ( temp->p[f] == NULL) { //If the mask creation fails...
            	temp->num = f; //We change the number of allocated masks to the number of succesfully allocated masks...
               free_dynamic_mask(temp); //and we call our function to deallocate the dynamic masks structure...
            	return NULL; //then we tell that we have failed...
            }

            break;

			case DAT_BITMAP:
         	temp->p[f] = create_ppcol_mask(dat[f].dat); //We create the mask for that sprite...

            if ( temp->p[f] == NULL) { //If the mask creation fails...
            	temp->num = f; //We change the number of allocated masks to the number of succesfully allocated masks...
               free_dynamic_mask(temp); //and we call our function to deallocate the dynamic masks structure...
            	return NULL; //then we tell that we have failed...
            }

            break;

      	default:
         	// Oh dear, didn't find graphic data...
            break;
      }
   }

   return temp;
}
