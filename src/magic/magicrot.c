/*         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      "Magic" sprite rotation routines.
 *
 *      By Shawn Hargreaves.
 *
 *      Flipping routines by Andrew Geers.
 *
 *      Optimized by Sven Sandberg.
 *
 *      Translucency hacked in by Peter Wang, with help from the C
 *      version of draw_trans_sprite, by Michael Bukin.
 *
 *      Hacked for "magic" sprites (i.e. make it use the 24 bpp
 *      routines, and divide bitmap width by 3).  Warning: some
 *      of these functions might not work, as I don't need them.
 * 
 *	Hacked again for lighting rather than translucency.
 *
 *	Hacked again to unifying the codebase.
 *
 *      See readme.txt for copyright information.
 */


#include <allegro.h>
#include <allegro/internal/aintern.h>



/*
 * Opaque.
 */

#define INT_COLOR_COMMA			
#define COMMA_INT_COLOR			
#define COLOR_COMMA			
#define COMMA_COLOR			
#define DRAW_SCANLINE			opaque_draw_scanline
#define PARALLELOGRAM_MAP		opaque_parallelogram_map
#define PARALLELOGRAM_MAP_STANDARD	opaque_parallelogram_map_standard
#define _PIVOT_SCALED_SPRITE_FLIP	opaque_pivot_scaled_sprite_flip
#define _ROTATE_SCALED_SPRITE_FLIP	opaque_rotate_scaled_sprite_flip

/* exported */
#define PIVOT_SCALED_SPRITE		pivot_scaled_magic_sprite
#define PIVOT_SPRITE			pivot_magic_sprite
#define PIVOT_SCALED_SPRITE_V_FLIP	pivot_scaled_magic_sprite_v_flip
#define PIVOT_SPRITE_V_FLIP		pivot_magic_sprite_v_flip
#define ROTATE_SCALED_SPRITE		rotate_scaled_magic_sprite
#define ROTATE_SPRITE			rotate_magic_sprite
#define ROTATE_SCALED_SPRITE_V_FLIP	rotate_scaled_magic_sprite_v_flip
#define ROTATE_SPRITE_V_FLIP		rotate_magic_sprite_v_flip

#include "magicrot.inc"

#undef INT_COLOR_COMMA			
#undef COMMA_INT_COLOR			
#undef COLOR_COMMA			
#undef COMMA_COLOR			
#undef DRAW_SCANLINE
#undef PARALLELOGRAM_MAP
#undef PARALLELOGRAM_MAP_STANDARD
#undef _PIVOT_SCALED_SPRITE_FLIP
#undef _ROTATE_SCALED_SPRITE_FLIP
#undef PIVOT_SCALED_SPRITE
#undef PIVOT_SPRITE
#undef PIVOT_SCALED_SPRITE_V_FLIP
#undef PIVOT_SPRITE_V_FLIP
#undef ROTATE_SCALED_SPRITE
#undef ROTATE_SPRITE
#undef ROTATE_SCALED_SPRITE_V_FLIP
#undef ROTATE_SPRITE_V_FLIP



/*
 * Translucent.
 */

#define INT_COLOR_COMMA			
#define COMMA_INT_COLOR			
#define COLOR_COMMA			
#define COMMA_COLOR			
#define BLENDER				COLOR_MAP*
#define MAKE_BLENDER()			color_map
#define BLEND(b,o,n)					\
((b)->data[(o) & 0xFF][(n) & 0xFF]  		      |	\
 (b)->data[(o) >> 8 & 0xFF][(n) >> 8 & 0xFF] << 8     |	\
 (b)->data[(o) >> 16 & 0xFF][(n) >> 16 & 0xFF] << 16)

#define DRAW_SCANLINE			trans_draw_scanline
#define PARALLELOGRAM_MAP		trans_parallelogram_map
#define PARALLELOGRAM_MAP_STANDARD	trans_parallelogram_map_standard
#define _PIVOT_SCALED_SPRITE_FLIP	trans_pivot_scaled_sprite_flip
#define _ROTATE_SCALED_SPRITE_FLIP	trans_rotate_scaled_sprite_flip

/* exported */
#define PIVOT_SCALED_SPRITE		pivot_scaled_trans_magic_sprite
#define PIVOT_SPRITE			pivot_trans_magic_sprite
#define PIVOT_SCALED_SPRITE_V_FLIP	pivot_scaled_trans_magic_sprite_v_flip
#define PIVOT_SPRITE_V_FLIP		pivot_trans_magic_sprite_v_flip
#define ROTATE_SCALED_SPRITE		rotate_scaled_trans_magic_sprite
#define ROTATE_SPRITE			rotate_trans_magic_sprite
#define ROTATE_SCALED_SPRITE_V_FLIP	rotate_scaled_trans_magic_sprite_v_flip
#define ROTATE_SPRITE_V_FLIP		rotate_trans_magic_sprite_v_flip

#include "magicrot.inc"

#undef INT_COLOR_COMMA			
#undef COMMA_INT_COLOR			
#undef COLOR_COMMA			
#undef COMMA_COLOR			
#undef BLENDER
#undef MAKE_BLENDER
#undef BLEND
#undef DRAW_SCANLINE
#undef PARALLELOGRAM_MAP
#undef PARALLELOGRAM_MAP_STANDARD
#undef _PIVOT_SCALED_SPRITE_FLIP
#undef _ROTATE_SCALED_SPRITE_FLIP
#undef PIVOT_SCALED_SPRITE
#undef PIVOT_SPRITE
#undef PIVOT_SCALED_SPRITE_V_FLIP
#undef PIVOT_SPRITE_V_FLIP
#undef ROTATE_SCALED_SPRITE
#undef ROTATE_SPRITE
#undef ROTATE_SCALED_SPRITE_V_FLIP
#undef ROTATE_SPRITE_V_FLIP



/*
 * Lit.
 */

#define INT_COLOR_COMMA			int color,
#define COMMA_INT_COLOR			,int color
#define COLOR_COMMA			color,
#define COMMA_COLOR			,color
#define BLENDER				unsigned char*
#define MAKE_BLENDER()			(color_map->data[color & 0xFF])
#define BLEND(b,c,ignore)		((b[c & 0xFF]) | (b[(c >> 8) & 0xFF] << 8) | (b[(c >> 16) & 0xFF] << 16))

#define DRAW_SCANLINE			lit_draw_scanline
#define PARALLELOGRAM_MAP		lit_parallelogram_map
#define PARALLELOGRAM_MAP_STANDARD	lit_parallelogram_map_standard
#define _PIVOT_SCALED_SPRITE_FLIP	lit_pivot_scaled_sprite_flip
#define _ROTATE_SCALED_SPRITE_FLIP	lit_rotate_scaled_sprite_flip

/* exported */
#define PIVOT_SCALED_SPRITE		pivot_scaled_lit_magic_sprite
#define PIVOT_SPRITE			pivot_lit_magic_sprite
#define PIVOT_SCALED_SPRITE_V_FLIP	pivot_scaled_lit_magic_sprite_v_flip
#define PIVOT_SPRITE_V_FLIP		pivot_lit_magic_sprite_v_flip
#define ROTATE_SCALED_SPRITE		rotate_scaled_lit_magic_sprite
#define ROTATE_SPRITE			rotate_lit_magic_sprite
#define ROTATE_SCALED_SPRITE_V_FLIP	rotate_scaled_lit_magic_sprite_v_flip
#define ROTATE_SPRITE_V_FLIP		rotate_lit_magic_sprite_v_flip

#include "magicrot.inc"
