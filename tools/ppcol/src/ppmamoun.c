#include <stdlib.h>
#include <allegro.h>
#include "ppcol.h"

int check_ppmask_collision_amount_normal(PPCOL_MASK *mask1, PPCOL_MASK *mask2, int x1, int y1, int x2, int y2, int addx, int addy)
{
 int dx1, dx2, dy1, dy2; //We will use this deltas...
 int py; //This will store the Y position...
 int maxh; //This will store the maximum height...

 int chunkm1, chunkm2; //This will store the number of the current chunk in the masks...
 int chunkm1p, chunkm2p; //This will store the position in the current chunk of the masks...

 int amount=0, curr_amount;

 RESTART:
 curr_amount=0;
 //First we do normal bounding box collision detection...
 if( !check_bb_collision(mask1, mask2, x1,y1, x2,y2) ) //If there is not bounding box collision...
   return amount;

 //Now we do pixel perfect collision detection...

 //First we need to see how much we have to shift the coordinates of the masks...
 if(x1>x2) {
   dx1=0;      //don't need to shift mask 1.
   dx2=x1-x2;  //shift mask 2 left. Why left? Because we have the mask 1 being on the right of the mask 2, so we have to move mask 2 to the left to do the proper pixel perfect collision...
   } else {
   dx1=x2-x1;  //shift mask 1 left.
   dx2=0;      //don't need to shift mask 2.
   }
 chunkm1 = dx1/32; //This sets the starting chunk number...
 chunkm2 = dx2/32;
 chunkm1p = dx1 - (32*chunkm1); //This sets the value we have to shift the chunks of this mask...
 chunkm2p = dx2 - (32*chunkm2);

 if(y1>y2) {
   dy1=0;
   dy2=y1-y2;  //we need to move this many rows up mask 2. Why up? Because we have mask 1 being down of mask 2, so we have to move mask 2 up to do the proper pixel perfect collision detection...
   } else {
   dy1=y2-y1;  //we need to move this many rows up mask 1.
   dy2=0;
   }
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
      while( (mask1->sp_mask[dy1+py][chunkm1] << chunkm1p) & (mask2->sp_mask[dy2+py][chunkm2] << chunkm2p) ) { //If bits of the mask coincides, we have a collision!
        x1+=addx;
        y1+=addy;
        chunkm1p-=addx;
        if(chunkm1p<0) {
          chunkm1p=31;
          chunkm1--; //Ok
          } else {
          if(chunkm1p>31) {
            chunkm1p=0;
            chunkm1++; //Ok
            }
          }
        dy1-=addy;
        curr_amount++;
        if( ((unsigned)chunkm1>(unsigned)mask1->max_chunk) || ((unsigned)(dy1+py)>=(unsigned)mask1->h) ) {
          amount+=curr_amount;
          goto RESTART;
          }
        }
      if(curr_amount) {
        amount+=curr_amount;
        goto RESTART;
        }
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
 return amount;
 }


