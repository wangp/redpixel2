/* 
 *  VAR.C - part of the EGG system.
 *
 *  Variable getting/setting routines.
 *
 *  By Shawn Hargreaves.
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "egg.h"



/* list of default settings */
typedef struct DEFAULT_VARIABLE
{
   char *name;
   double value;
} DEFAULT_VARIABLE;



static DEFAULT_VARIABLE default_variable[] =
{
   { "x",      0   },
   { "y",      0   },
   { "z",      0   },
   { "size",   1   },
   { "focus",  1   },
   { "aa",     0   },
   { "r",      255 },
   { "g",      255 },
   { "b",      255 },
   { "a",      255 },
   { "add",    1   },
   { "mul",    0   },
   { "sub",    0   },
   { "_scale", 1   },
   { "_dist",  100 },
   { "_fov",   1   }
};



/* get_egg_variable:
 *  Accesses a particle variable.
 */
double get_egg_variable(EGG_PARTICLE *part, EGG_PARTICLE *other, EGG *egg, char *name)
{
   EGG_VARIABLE *var;
   int i;

   if (name[0] == '_') {
      if (other) {
	 var = other->var;
	 name++;
      }
      else if (egg)
	 var = egg->var;
      else
	 var = NULL;
   }
   else {
      if (part)
	 var = part->var;
      else
	 var = NULL;
   }

   while (var) {
      if (strcmp(var->name, name) == 0) {
	 return var->val;
      }

      var = var->next;
   }

   for (i=0; i<sizeof(default_variable)/sizeof(DEFAULT_VARIABLE); i++) {
      if (strcmp(default_variable[i].name, name) == 0)
	 return default_variable[i].value;
   }

   if ((part) && (strcmp(name, "id") == 0))
      return part->num;

   if ((egg) && (strcmp(name, "_frame") == 0))
      return egg->frame;

   if ((egg) && (strcmp(name, "_count") == 0))
      return egg->part_count;

   return 0;
}



/* set_egg_variable:
 *  Sets a particle variable.
 */
void set_egg_variable(EGG_PARTICLE *part, EGG_PARTICLE *other, EGG *egg, char *name, double value)
{
   EGG_VARIABLE *var, **pvar;

   if (name[0] == '_') {
      if (other) {
	 pvar = &other->var;
	 name++;
      }
      else {
	 if (!egg)
	    return;

	 pvar = &egg->var; 
      }
   }
   else {
      if (!part)
	 return;

      pvar = &part->var;
   }

   var = *pvar;

   while (var) {
      if (strcmp(var->name, name) == 0) {
	 var->val = value;
	 return;
      }

      var = var->next;
   }

   var = malloc(sizeof(EGG_VARIABLE));

   var->name = malloc(strlen(name)+1);
   strcpy(var->name, name);

   var->val = value;

   var->next = *pvar;
   *pvar = var;
}



