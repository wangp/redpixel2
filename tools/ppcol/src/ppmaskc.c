#include <stdlib.h>
#include <allegro.h>

#include "ppcol.h"

//This is the mask creation routine. Given a sprite it creates a mask for that
//sprite.
//Returns a pointer to a PPCOL_MASK or NULL if there is a problem...
PPCOL_MASK *create_ppcol_mask(BITMAP *sprite)
{
 PPCOL_MASK *maskout;
 int INTVAR, INTVAR2, INTVAR3; //We will use this temporary...

 if(!sprite)   //User passes an uninitialized sprite...
   return NULL;

 //We have to allocate the memory for the structure fields...
 //               Size of structure  + size of pointers to rows of chunks * number of sprite rows.
 maskout = malloc(sizeof(PPCOL_MASK) + (sizeof(unsigned long int *) * sprite->h) );
 if(!maskout)  //maskout structure allocation failed!
   return NULL;

 //Now we initialize some of the fields of the structure...
 maskout->h = sprite->h;
 maskout->w = sprite->w;
 //                 not divisible by 32?       yes:              no:
 maskout->max_chunk = ( (sprite->w%32) ? (sprite->w/32) : (sprite->w/32-1) ); //Chunks are 32 pixels... so w/32... max_chunks are the maximum number of chunks -1, so if divisible by 32: w/32-1, if not divisible by 32: w/32.

 //Now that we have some more data, we allocate the mask data...
 maskout->dat = malloc( (maskout->max_chunk+1) * 4 * sprite->h );
 if(!maskout->dat) //data allocation failed!
   {
    free(maskout); //Free the structure kernel data.
    return NULL;
   }

 //We are going to initialize the array of pointers to the begining of each row of chunks...
 maskout->sp_mask[0] = maskout->dat; //The first pointer points to the begining of the data...
 for(INTVAR=1; INTVAR < maskout->h; INTVAR++) //Now initialize all the other pointers...
    maskout->sp_mask[INTVAR]=maskout->sp_mask[INTVAR-1] + (maskout->max_chunk+1); //the position of the previous pointer + number of the chunks.

 //Now we have a proper mask structure allocated and mostly initialized, but the mask data has garbage! We have to initialize it to 0's:
 for(INTVAR=maskout->h-1; INTVAR>=0; INTVAR--) {
    for(INTVAR2=maskout->max_chunk; INTVAR2>=0; INTVAR2--) {
       maskout->sp_mask[INTVAR][INTVAR2]=0;
       }
    }

 //Now we have to create the bit mask array for the sprite:
 for(INTVAR=0; INTVAR < maskout->h; INTVAR++) { //Row loop...
    for(INTVAR2=0; INTVAR2 <= maskout->max_chunk; INTVAR2++) { //Chunk loop...
       for(INTVAR3=INTVAR2*32; INTVAR3 < (INTVAR2*32+32); INTVAR3++) { //Loop between the pixels that the current chunk covers...
          if( INTVAR3<sprite->w ) { //Check if we are in range in the X axis... because if we pass it, getpixel would return a bad return value...
            if( getpixel(sprite, INTVAR3, INTVAR) != bitmap_mask_color(sprite) ) { //The pixel is not a mask color... so we have to check for collision, so we add it to the bitarray in the current chunk...
              //maskout->sp_mask[INTVAR][INTVAR2] += 0x80000000 >> ( INTVAR3 - (INTVAR2+1)*32 ); //This was the original from Neil.
              maskout->sp_mask[INTVAR][INTVAR2] |= 0x80000000 >> ( INTVAR3 - (INTVAR2)*32 ); //This seems more correct... //0x80000000 == 0b10000000000000000000000000000000, we shift this bit to the right position where it must be in the current chunk, this indicates that there is a pixel that we must check for collision.
              }
            }
          }
       }
    }

 //We have allocated the memory for a mask for the given sprite, we have initialized it and filled the mask bit data with correct data... so this is the end of this function! Don't you think?
 return maskout; //Return the pointer to the mask we have created.
 }


//This is the mask deallocation routine, pass it a PPCOL_MASK pointer and it
//will be deallocated from memory. If the passed pointer was already
//deallocated or not allocated at all, this function does nothing.
void destroy_ppcol_mask(PPCOL_MASK *mask)
{
 if(mask) { //Mask is not NULL? Mask is allocated?
   if(mask->dat) free(mask->dat); //Deallocate the data if it was not deallocated...
   free(mask); //Deallocate the mask structure...
   }
 }


//This is the pixel perfect collision detection routine. Pass it 2 properly
//initialized sprite masks and then their respective X, Y positions.
//Returns false if there is not collision and true if there is a collision.
int check_ppmask_collision_normal(PPCOL_MASK *mask1, PPCOL_MASK *mask2, int x1, int y1, int x2, int y2)
{
 int dx1, dx2, dy1, dy2; //We will use this deltas...
 int py; //This will store the Y position...
 int maxh; //This will store the maximum height...

 int chunkm1, chunkm2; //This will store the number of the current chunk in the masks...
 int chunkm1p, chunkm2p; //This will store the position in the current chunk of the masks...

 //First we do normal bounding box collision detection...
 if( !check_bb_collision(mask1, mask2, x1,y1, x2,y2) ) //If there is not bounding box collision...
   return 0; //return that there is not collision...

 //Now we do pixel perfect collision detection...

 //First we need to see how much we have to shift the coordinates of the masks...
 if(x1>x2) {
   dx1=0;      //don't need to shift mask 1.
   dx2=x1-x2;  //shift mask 2 left. Why left? Because we have the mask 1 being on the right of the mask 2, so we have to move mask 2 to the left to do the proper pixel perfect collision...
   } else {
   dx1=x2-x1;  //shift mask 1 left.
   dx2=0;      //don't need to shift mask 2.
   }
 if(y1>y2) {
   dy1=0;
   dy2=y1-y2;  //we need to move this many rows up mask 2. Why up? Because we have mask 1 being down of mask 2, so we have to move mask 2 up to do the proper pixel perfect collision detection...
   } else {
   dy1=y2-y1;  //we need to move this many rows up mask 1.
   dy2=0;
   }

 //We need to initialize this values first...
 chunkm1 = dx1/32; //This sets the starting chunk number...
 chunkm2 = dx2/32;
 chunkm1p = dx1 - (32*chunkm1); //This sets the value we have to shift the chunks of this mask...
 chunkm2p = dx2 - (32*chunkm2);

 //This will calculate the maximum height that we will reach...
 if(mask1->h-dy1 > mask2->h-dy2) {
   maxh=mask2->h-dy2;
   } else {
   maxh=mask1->h-dy1;
   }
 maxh--;

 //Now we start the actual pixel perfect collision detection loop...
 while( (chunkm1<=mask1->max_chunk) && (chunkm2<=mask2->max_chunk) ) { //While there remains more chunks in the 2 masks we continue doing the ppcol detection...
   for(py=maxh;py>=0;py--) { //Test for all rows...
     if( (mask1->sp_mask[dy1+py][chunkm1] << chunkm1p) & (mask2->sp_mask[dy2+py][chunkm2] << chunkm2p) ) //If bits of the mask coincides, we have a collision!
       return 1; //Return that there is a collision!
     }
   //Now we have to move to the next chunk...
   if( (!chunkm1p) && (!chunkm2p) ) { //In case both masks are lined up on the x axis...
     chunkm1++; //Increment both chunks...
     chunkm2++;
     } else {
     if(!chunkm1p) { //If the mask 1 don't need shifting...
       chunkm2++;
       chunkm1p = 32 - chunkm2p;
       chunkm2p = 0;
       } else {
       if(!chunkm2p) {
         chunkm1++;
         chunkm2p = 32 - chunkm1p;
         chunkm1p = 0;
         }
       }
     }

   } //There are no more chunks to test...
 //We haven't found a collision... so, return 0.
 return 0;
 }

