/* 
 *  INTERP.C - part of the EGG system.
 *
 *  Main script interpreter.
 *
 *  By Shawn Hargreaves.
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "egg.h"



/* the current particle */
static EGG_PARTICLE *the_particle;
static EGG_PARTICLE *the_other;
static EGG *the_egg;



/* variable_getter:
 *  Callback for reading variables out of the particle structure.
 */
static double variable_getter(char *name)
{
   if (the_particle)
      return get_egg_variable(the_particle, the_other, the_egg, name);

   return 0;
}



/* evalulate_expression:
 *  Wrapper for the expression evaluator module.
 */
static double evalulate_expression(char *exp, EGG_PARTICLE *part, EGG_PARTICLE *other, EGG *egg, int line, char *error)
{
   double ret;
   int err;

   the_particle = part;
   the_other = other;
   the_egg = egg;

   ret = evaluate(exp, &err, variable_getter);

   if (err) {
      sprintf(error, "Error in expression at line %d", line);
      return 0;
   }

   return ret;
}



/* create_particle:
 *  Wakes up a new particle.
 */
static void create_particle(EGG *egg, EGG_PARTICLE *other, EGG_COMMAND *cmd, int num, char *error)
{
   EGG_TYPE *type;
   EGG_PARTICLE *part;
   int i;

   type = egg->type;

   while (type) {
      if (strcmp(type->name, cmd->var) == 0) {
	 for (i=0; (i<num) && (!error[0]); i++) {
	    part = malloc(sizeof(EGG_PARTICLE));

	    part->num = ++egg->part_num;
	    part->type = type;
	    part->var = NULL;
	    part->next = NULL;

	    if (egg->part) {
	       EGG_PARTICLE *p = egg->part;

	       while (p->next)
		  p = p->next;

	       p->next = part;
	    }
	    else
	       egg->part = part;

	    egg->part_count++;

	    process_egg_cmd(egg, part, NULL, type->cmd, TRUE, FALSE, error);

	    if (!error[0])
	       process_egg_cmd(egg, part, other, cmd->cmd, TRUE, TRUE, error);
	 }

	 return;
      }

      type = type->next;
   }

   sprintf(error, "Unknown type '%s' at line %d", cmd->var, cmd->line);
}



/* process_egg_cmd:
 *  Interprets a list of commands, applying them to the specified particle.
 */
int process_egg_cmd(EGG *egg, EGG_PARTICLE *part, EGG_PARTICLE *other, EGG_COMMAND *cmd, int init, int set, char *error)
{
   double ret;

   while ((cmd) && (!error[0])) {

      switch (cmd->type) {

	 case EGG_COMMAND_INIT:
	    /* set variables to their initial values */
	    if ((!part) && (cmd->var[0] != '_')) {
	       strcpy(error, "Cannot initialise local variable at global scope");
	    }
	    else if (init) {
	       ret = evalulate_expression(cmd->exp, part, other, egg, cmd->line, error);

	       if (!error[0])
		  set_egg_variable(part, other, egg, cmd->var, ret);
	    }
	    break;

	 case EGG_COMMAND_SET:
	    /* modify variables */
	    if ((!part) && (cmd->var[0] != '_')) {
	       strcpy(error, "Cannot set local variable at global scope");
	    }
	    else if (set) {
	       ret = evalulate_expression(cmd->exp, part, other, egg, cmd->line, error);

	       if (!error[0])
		  set_egg_variable(part, other, egg, cmd->var, ret);
	    }
	    break;

	 case EGG_COMMAND_IF:
	    /* process conditionals */
            if (!init) {
	       ret = evalulate_expression(cmd->exp, part, other, egg, cmd->line, error);

	       if ((!error[0]) && (ret != 0)) {
	          if (cmd->cmd)
		     if (process_egg_cmd(egg, part, other, cmd->cmd, init, set, error) != 0)
		        return TRUE;
	       }
	       else {
	          if (cmd->cmd2)
		     if (process_egg_cmd(egg, part, other, cmd->cmd2, init, set, error) != 0)
		        return TRUE;
	       }
            }
	    break;

	 case EGG_COMMAND_WHILE:
	    /* process loops */
            if (!init) {
	       while ((ret = evalulate_expression(cmd->exp, part, other, egg, cmd->line, error))) {
                  if (cmd->cmd)
		     if (process_egg_cmd(egg, part, other, cmd->cmd, init, set+789, error) != 0)
		        return TRUE;
               }
            }
	    break;

	 case EGG_COMMAND_LAY:
	    /* create new particles */
	    if (set) {
	       if (cmd->exp)
		  ret = evalulate_expression(cmd->exp, part, other, egg, cmd->line, error);
	       else
		  ret = 1;

	       if (!error[0])
		  create_particle(egg, part, cmd, (int)ret, error);
	    }
	    break;

	 case EGG_COMMAND_DIE:
	    /* kill this particle */
	    if (part)
	       return TRUE;
	    break;

	 case EGG_COMMAND_SRAND:
            /* initialize the random number generator */
            srandom(evalulate_expression(cmd->exp, part, other, egg, cmd->line, error));
	    break;
      }

      cmd = cmd->next;
   }

   return FALSE;
}



/* update_egg:
 *  Advances the entire animation to the next frame.
 */
int update_egg(EGG *egg, char *error)
{
   EGG_PARTICLE *part, *prev_part, *next_part;
   EGG_VARIABLE *var, *next_var;
   int ret;

   error[0] = 0;

   part = egg->part;
   prev_part = NULL;

   while ((part) && (!error[0])) {
      ret = process_egg_cmd(egg, part, NULL, part->type->cmd, FALSE, TRUE, error);

      if (!error[0]) {
	 if (ret != 0) {
	    var = part->var;

	    while (var) {
	       if (var->name)
		  free(var->name);

	       next_var = var->next;
	       free(var);
	       var = next_var;
	    }

	    if (prev_part)
	       next_part = prev_part->next = part->next;
	    else
	       next_part = egg->part = part->next;

	    egg->part_count--;

	    free(part);
	    part = next_part;
	 }
	 else {
	    prev_part = part;
	    part = part->next;
	 }
      }
   }

   if (error[0])
      return TRUE;

   egg->frame++;

   return FALSE;
}

