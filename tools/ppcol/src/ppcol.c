#include "ppcol.h"

//Pass 2 Allegro bitmaps and their respective positions and this function
//returns true if there is a collision and false if theres not.
//The 2 bitmaps must be memory bitmaps of the same color depth.
int check_pp_collision_normal(BITMAP *spr1, BITMAP *spr2, int x1, int y1, int x2, int y2)
{
 int dx1, dx2, dy1, dy2; //We will use this deltas...
 int fx,fy,sx1,sx2; //Also we will use this starting/final position variables...
 int maxw, maxh; //And also this variables saying what is the maximum width and height...
 int depth; //This will store the color depth value...
 char CHARVAR; //We will use these to store the transparent color for the sprites...
 short SHORTVAR;
 long LONGVAR;

 if( !check_bb_collision(spr1, spr2, x1,y1, x2,y2) ) return 0; //If theres not a bounding box collision, it is impossible to have a pixel perfect collision right? So, we return that theres not collision...

 //First we need to see how much we have to shift the coordinates of the sprites...
 if(x1>x2) {
   dx1=0;      //don't need to shift sprite 1.
   dx2=x1-x2;  //shift sprite 2 left. Why left? Because we have the sprite 1 being on the right of the sprite 2, so we have to move sprite 2 to the left to do the proper pixel perfect collision...
   } else {
   dx1=x2-x1;  //shift sprite 1 left.
   dx2=0;      //don't need to shift sprite 2.
   }
 if(y1>y2) {
   dy1=0;
   dy2=y1-y2;  //we need to move this many rows up sprite 2. Why up? Because we have sprite 1 being down of sprite 2, so we have to move sprite 2 up to do the proper pixel perfect collision detection...
   } else {
   dy1=y2-y1;  //we need to move this many rows up sprite 1.
   dy2=0;
   }

 //Then, we have to see how far we have to go, we do this seeing the minimum height and width between the 2 sprites depending in their positions:
 if(spr1->w-dx1 > spr2->w-dx2) {
   maxw=spr2->w-dx2;
   } else {
   maxw=spr1->w-dx1;
   }
 if(spr1->h-dy1 > spr2->h-dy2) {
   maxh=spr2->h-dy2;
   } else {
   maxh=spr1->h-dy1;
   }
 maxw--;
 maxh--;

 fy=dy1;
 fx=dx1;
 dy1+=maxh;
 dy2+=maxh;
 sx1=dx1+maxw;
 sx2=dx2+maxw;

 depth=bitmap_color_depth(spr1); //Get the bitmap depth...

 if(depth==8) {
   CHARVAR=bitmap_mask_color(spr1); //Get the transparent color of the sprites...
   for(; dy1>=fy; dy1--,dy2--) { //Go through lines...
      for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
         if((spr1->line[dy1][dx1]!=CHARVAR) && (spr2->line[dy2][dx2]!=CHARVAR)) return 1; //Both sprites don't have transparent color in that position, so, theres a collision and return collision detected!
         }
      }
   } else {
   if(depth==16 || depth==15) {
     SHORTVAR=bitmap_mask_color(spr1); //Get the transparent color of the sprites...
     for(; dy1>=fy; dy1--,dy2--) { //Go through lines...
        for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
           if(( ((short *)spr1->line[dy1])[dx1]!=SHORTVAR) && ( ((short *)spr2->line[dy2])[dx2]!=SHORTVAR)) return 1; //Both sprites don't have transparent color in that position, so, theres a collision and return collision detected!
           }
        }
     } else {
     if(depth==32) {
       LONGVAR=bitmap_mask_color(spr1); //Get the transparent color of the sprites...
       for(; dy1>=fy; dy1--,dy2--) { //Go through lines...
          for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
             if(( ((long *)spr1->line[dy1])[dx1]!=LONGVAR) && ( ((long *)spr2->line[dy2])[dx2]!=LONGVAR)) return 1; //Both sprites don't have transparent color in that position, so, theres a collision and return collision detected!
             }
          }
       } else {
       if(depth==24) {
         CHARVAR=bitmap_mask_color(spr1)>>16; //if the order is RGB, this will contain B...
         SHORTVAR=bitmap_mask_color(spr1)&0xffff; //if the order is RGB, this will contain GR...
         for(; dy1>=fy; dy1--,dy2--) { //Go through lines...
            for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
               if( (*((short *)(spr1->line[dy1]+(dx1)*3))!=SHORTVAR) && (spr1->line[dy1][(dx1)*3+2]!=CHARVAR)  &&  (*((short *)(spr2->line[dy2]+(dx2)*3))!=SHORTVAR) && (spr2->line[dy2][(dx2)*3+2]!=CHARVAR) ) return 1; //Both sprites don't have transparent color in that position, so, theres a collision and return collision detected!
               //I have tryed to avoid the above multiplications but it seems that GCC optimizes better than I :-))
               }
            }
         }
       }
     }
   }

 //If we have reached here it means that theres not a collision:
 return 0; //Return no collision.
 }
