#include "ppcol.h"

//The direction is according to the first sprite.
int check_pp_collision_amount_normal(BITMAP *spr1, BITMAP *spr2, int x1, int y1, int x2, int y2, int addx, int addy)
{
 int amount;
 int curr_amount;

 int dx1, dx2, dy1, dy2; //We will use this deltas...
 int fx,fy,sx1,sx2,sy1,sy2; //Also we will use this starting/final position variables...
 int maxw, maxh; //And also this variables saying what is the maximum width and height...
 int depth; //This will store the color depth value...
 char CHARVAR; //We will use these to store the transparent color for the sprites...
 short SHORTVAR;
 long LONGVAR;

 fx=fy=sx1=sx2=sy1=sy2=0; //This is to avoid the warning that this variables might be used uninitialized in this function. Is there a way to avoid this using a preprocesor statement or something like that?

 amount=0;

 depth=bitmap_color_depth(spr1); //Get the bitmap depth...

 //Initialization of variables that will not change during scan...
 if(addx==0) {
   if(x1>x2) {
     dx1=0;      //don't need to shift sprite 1.
     dx2=x1-x2;  //shift sprite 2 left. Why left? Because we have the sprite 1 being on the right of the sprite 2, so we have to move sprite 2 to the left to do the proper pixel perfect collision...
     } else {
     dx1=x2-x1;  //shift sprite 1 left.
     dx2=0;      //don't need to shift sprite 2.
     }
   if(spr1->w-dx1 > spr2->w-dx2) {
     maxw=spr2->w-dx2;
     } else {
     maxw=spr1->w-dx1;
     }
   maxw--;
   fx=dx1;
   sx1=dx1+maxw;
   sx2=dx2+maxw;
   }

 if(addy==0) {
   if(y1>y2) {
     dy1=0;
     dy2=y1-y2;  //we need to move this many rows up sprite 2. Why up? Because we have sprite 1 being down of sprite 2, so we have to move sprite 2 up to do the proper pixel perfect collision detection...
     } else {
     dy1=y2-y1;  //we need to move this many rows up sprite 1.
     dy2=0;
     }
   if(spr1->h-dy1 > spr2->h-dy2) {
     maxh=spr2->h-dy2;
     } else {
     maxh=spr1->h-dy1;
     }
   maxh--;
   fy=dy1;
   sy1=dy1+maxh;
   sy2=dy2+maxh;
   }



 RESTART:
 curr_amount=0;
 if( !check_bb_collision(spr1, spr2, x1,y1, x2,y2) ) return amount;

 //Initialization of variables that have changed.
 if(addx) {
   if(x1>x2) {
     dx1=0;      //don't need to shift sprite 1.
     dx2=x1-x2;  //shift sprite 2 left. Why left? Because we have the sprite 1 being on the right of the sprite 2, so we have to move sprite 2 to the left to do the proper pixel perfect collision...
     } else {
     dx1=x2-x1;  //shift sprite 1 left.
     dx2=0;      //don't need to shift sprite 2.
     }
   if(spr1->w-dx1 > spr2->w-dx2) {
     maxw=spr2->w-dx2;
     } else {
     maxw=spr1->w-dx1;
     }
   maxw--;
   fx=dx1;
   sx1=dx1+maxw;
   sx2=dx2+maxw;
   }

 if(addy) {
   if(y1>y2) {
     dy1=0;
     dy2=y1-y2;  //we need to move this many rows up sprite 2. Why up? Because we have sprite 1 being down of sprite 2, so we have to move sprite 2 up to do the proper pixel perfect collision detection...
     } else {
     dy1=y2-y1;  //we need to move this many rows up sprite 1.
     dy2=0;
     }
   if(spr1->h-dy1 > spr2->h-dy2) {
     maxh=spr2->h-dy2;
     } else {
     maxh=spr1->h-dy1;
     }
   maxh--;
   fy=dy1;
   sy1=dy1+maxh;
   sy2=dy2+maxh;
   }


 if(depth==8) {
   CHARVAR=bitmap_mask_color(spr1); //Get the transparent color of the sprites...
   for(dy1=sy1,dy2=sy2; dy1>=fy; dy1--,dy2--) { //Go through lines...
      for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
         while((spr1->line[dy1][dx1]!=CHARVAR) && (spr2->line[dy2][dx2]!=CHARVAR)) {
           x1+=addx;
           y1+=addy;
           dx1-=addx;
           dy1-=addy;
           curr_amount++;
           if( ((unsigned)dx1>=(unsigned)spr1->w) || ((unsigned)dy1>=(unsigned)spr1->h) ) {
             amount+=curr_amount;
             goto RESTART;
             }
           }
         if(curr_amount) {
           amount+=curr_amount;
           goto RESTART;
           }
         }
      }
   } else {
   if(depth==16 || depth==15) {
     SHORTVAR=bitmap_mask_color(spr1); //Get the transparent color of the sprites...
     for(dy1=sy1,dy2=sy2; dy1>=fy; dy1--,dy2--) { //Go through lines...
        for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
           while( (((short *)spr1->line[dy1])[dx1]!=SHORTVAR) && (((short *)spr2->line[dy2])[dx2]!=SHORTVAR) ) {
             x1+=addx;
             y1+=addy;
             dx1-=addx;
             dy1-=addy;
             curr_amount++;
             if( ((unsigned)dx1>=(unsigned)spr1->w) || ((unsigned)dy1>=(unsigned)spr1->h) ) {
               amount+=curr_amount;
               goto RESTART;
               }
             }
           if(curr_amount) {
             amount+=curr_amount;
             goto RESTART;
             }
           }
        }
     } else {
     if(depth==32) {
       LONGVAR=bitmap_mask_color(spr1); //Get the transparent color of the sprites...
       for(dy1=sy1,dy2=sy2; dy1>=fy; dy1--,dy2--) { //Go through lines...
          for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
             while( (((long *)spr1->line[dy1])[dx1]!=LONGVAR) && (((long *)spr2->line[dy2])[dx2]!=LONGVAR) ) {
               x1+=addx;
               y1+=addy;
               dx1-=addx;
               dy1-=addy;
               curr_amount++;
               if( ((unsigned)dx1>=(unsigned)spr1->w) || ((unsigned)dy1>=(unsigned)spr1->h) ) {
                 amount+=curr_amount;
                 goto RESTART;
                 }
               }
             if(curr_amount) {
               amount+=curr_amount;
               goto RESTART;
               }
             }
          }
       } else {
       if(depth==24) {
         CHARVAR=bitmap_mask_color(spr1)>>16; //if the order is RGB, this will contain B...
         SHORTVAR=bitmap_mask_color(spr1)&0xffff; //if the order is RGB, this will contain GR...
         for(dy1=sy1,dy2=sy2; dy1>=fy; dy1--,dy2--) { //Go through lines...
            for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
               while( (*((short *)(spr1->line[dy1]+(dx1)*3))!=SHORTVAR) && (spr1->line[dy1][(dx1)*3+2]!=CHARVAR)  &&  (*((short *)(spr2->line[dy2]+(dx2)*3))!=SHORTVAR) && (spr2->line[dy2][(dx2)*3+2]!=CHARVAR) ) {
                 x1+=addx;
                 y1+=addy;
                 dx1-=addx;
                 dy1-=addy;
                 curr_amount++;
                 if( ((unsigned)dx1>=(unsigned)spr1->w) || ((unsigned)dy1>=(unsigned)spr1->h) ) {
                   amount+=curr_amount;
                   goto RESTART;
                   }
                 }
               if(curr_amount) {
                 amount+=curr_amount;
                 goto RESTART;
                 }
               }
            }
         }
       }
     }
   }
 return amount;
 }

