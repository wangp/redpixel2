/*  _______         ____    __         ___    ___
 * \    _  \       \    /  \  /       \   \  /   /     '   '  '
 *  |  | \  \       |  |    ||         |   \/   |       .      .
 *  |  |  |  |      |  |    ||         ||\  /|  |
 *  |  |  |  |      |  |    ||         || \/ |  |       '  '  '
 *  |  |  |  |      |  |    ||         ||    |  |       .      .
 *  |  |_/  /        \  \__//          ||    |  |
 * /_______/edicated  \____/niversal  /__\  /____\usic /|  .  . astardisation
 *                                                    /  \
 *                                                   / .  \
 * clickrem.c - Click removal helpers.              / / \  \
 *                                                 | <  /   \_
 * By entheh.                                      |  \/ /\   /
 *                                                  \_  /  > /
 *                                                    | \ / /
 *                                                    |  ' /
 *                                                     \__/
 */

#include <stdlib.h>
#include <math.h>
#include "dumb.h"



typedef struct DUMB_CLICK DUMB_CLICK;


#define FACTOR_FIX_SHIFT 12


struct DUMB_CLICK_REMOVER
{
	DUMB_CLICK *click;
	int n_clicks;

	int offset;
};


struct DUMB_CLICK
{
	DUMB_CLICK *next;
	long pos;
	sample_t step;
};



DUMB_CLICK_REMOVER *dumb_create_click_remover(void)
{
	DUMB_CLICK_REMOVER *cr = malloc(sizeof(*cr));
	if (!cr) return NULL;

	cr->click = NULL;
	cr->n_clicks = 0;

	cr->offset = 0;

	return cr;
}



void dumb_record_click(DUMB_CLICK_REMOVER *cr, long pos, sample_t step)
{
	DUMB_CLICK *click;

	if (!cr || !step) return;

	click = malloc(sizeof(*click));
	if (!click) return;

	ASSERT(pos >= 0);

	click->pos = pos;
	click->step = step;

	click->next = cr->click;
	cr->click = click;
	cr->n_clicks++;
}



static DUMB_CLICK *mergesort(DUMB_CLICK *click, int n_clicks)
{
	int i;
	DUMB_CLICK *c1, *c2, **cp;

	if (n_clicks <= 1) return click;

	/* Split the list into two */
	c1 = click;
	cp = &c1;
	for (i = 0; i < n_clicks; i += 2) cp = &(*cp)->next;
	c2 = *cp;
	*cp = NULL;

	/* Sort the sublists */
	c1 = mergesort(c1, (n_clicks + 1) >> 1);
	c2 = mergesort(c2, n_clicks >> 1);

	/* Merge them */
	cp = &click;
	while (c1 && c2) {
		if (c1->pos > c2->pos) {
			*cp = c2;
			c2 = c2->next;
		} else {
			*cp = c1;
			c1 = c1->next;
		}
		cp = &(*cp)->next;
	}
	if (c2)
		*cp = c2;
	else
		*cp = c1;

	return click;
}



void dumb_remove_clicks(DUMB_CLICK_REMOVER *cr, sample_t *samples, long length, float halflife)
{
	DUMB_CLICK *click;
	long pos = 0;
	int offset;
	int factor;

	if (!cr) return;

	factor = (int)floor(pow(0.5, 1.0/halflife) * (1 << FACTOR_FIX_SHIFT));

	click = mergesort(cr->click, cr->n_clicks);
	cr->click = NULL;
	cr->n_clicks = 0;

	while (click) {
		DUMB_CLICK *next = click->next;
		ASSERT(click->pos <= length);
		offset = cr->offset;
		while (pos < click->pos) {
			/* If the offset is negative, add ((1 << FACTOR_FIX_SHIFT) - 1)
			 * to make sure it reaches zero.
			 */
			int round = (offset >> 31) & ((1 << FACTOR_FIX_SHIFT) - 1);
			samples[pos++] += offset;
			offset = (offset * factor + round) >> FACTOR_FIX_SHIFT;
		}
		cr->offset = offset - click->step;
		free(click);
		click = next;
	}

	offset = cr->offset;
	while (pos < length) {
		/* See above comment. */
		int round = (offset >> 31) & ((1 << FACTOR_FIX_SHIFT) - 1);
		samples[pos++] += offset;
		offset = (offset * factor + round) >> FACTOR_FIX_SHIFT;
	}
	cr->offset = offset;
}



void dumb_destroy_click_remover(DUMB_CLICK_REMOVER *cr)
{
	if (cr) {
		DUMB_CLICK *click = cr->click;
		while (click) {
			DUMB_CLICK *next = click->next;
			free(click);
			click = next;
		}
		free(cr);
	}
}



DUMB_CLICK_REMOVER **dumb_create_click_remover_array(int n)
{
	int i;
	DUMB_CLICK_REMOVER **cr;
	if (n <= 0) return NULL;
	cr = malloc(n * sizeof(*cr));
	if (!cr) return NULL;
	for (i = 0; i < n; i++) cr[i] = dumb_create_click_remover();
	return cr;
}



void dumb_remove_clicks_array(int n, DUMB_CLICK_REMOVER **cr, sample_t **samples, long length, float halflife)
{
	if (cr) {
		int i;
		for (i = 0; i < n; i++)
			dumb_remove_clicks(cr[i], samples[i], length, halflife);
	}
}



void dumb_destroy_click_remover_array(int n, DUMB_CLICK_REMOVER **cr)
{
	if (cr) {
		int i;
		for (i = 0; i < n; i++) dumb_destroy_click_remover(cr[i]);
		free(cr);
	}
}
