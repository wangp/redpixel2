/* 
 *  UNLOAD.C - part of the EGG system.
 *
 *  Memory freeing routines.
 *
 *  By Shawn Hargreaves.
 */


#include <stdlib.h>
#include <stdio.h>

#include "egg.h"



/* destroy_egg_cmd:
 *  Removes a list of particle commands from memory.
 */
void destroy_egg_cmd(EGG_COMMAND *cmd)
{
   EGG_COMMAND *next;

   while (cmd) {
      if (cmd->var)
	 free(cmd->var);

      if (cmd->exp)
	 free(cmd->exp);

      if (cmd->cmd)
	 destroy_egg_cmd(cmd->cmd);

      if (cmd->cmd2)
	 destroy_egg_cmd(cmd->cmd2);

      next = cmd->next;
      free(cmd);
      cmd = next;
   }
}



/* destroy_egg:
 *  Removes an animation object from memory.
 */
void destroy_egg(EGG *egg)
{
   EGG_PARTICLE *part, *next_part;
   EGG_VARIABLE *var, *next_var;
   EGG_TYPE *type, *next_type;

   if (egg) {
      part = egg->part;

      while (part) {
	 var = part->var;

	 while (var) {
	    if (var->name)
	       free(var->name);

	    next_var = var->next;
	    free(var);
	    var = next_var;
	 }

	 next_part = part->next;
	 free(part);
	 part = next_part;
      }

      type = egg->type;

      while (type) {
	 if (type->name)
	    free(type->name);

	 if (type->cmd)
	    destroy_egg_cmd(type->cmd);

	 next_type = type->next;
	 free(type);
	 type = type->next;
      }

      var = egg->var;

      while (var) {
	 if (var->name)
	    free(var->name);

	 next_var = var->next;
	 free(var);
	 var = next_var;
      }

      free(egg);
   }
}

