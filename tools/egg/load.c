/* 
 *  LOAD.C - part of the EGG system.
 *
 *  Parses an animation script, reading from a disk file or memory buffer.
 *
 *  By Shawn Hargreaves.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <allegro.h>

#include "egg.h"



/* where are we reading from? */
static PACKFILE *egg_file;

static unsigned char *egg_data;
static int egg_data_length;

static int egg_unget_char;

static int egg_line;



/* egg_error:
 *  Formats an error message.
 */
static void egg_error(char *error, char *msg, ...)
{
   char buf[256];

   va_list ap;
   va_start(ap, msg);
   vsprintf(buf, msg, ap);
   va_end(ap);

   sprintf(error, "%s at line %d", buf, egg_line);
}



/* do_getc:
 *  Reads the next byte of the input stream.
 */
static int do_getc()
{
   if (egg_unget_char) {
      int c = egg_unget_char;
      egg_unget_char = 0;
      return c;
   }

   if (egg_file)
      return pack_getc(egg_file);

   if ((egg_data) && (egg_data_length > 0)) {
      egg_data_length--;
      return *(egg_data++);
   }

   return EOF;
}



/* egg_getc:
 *  Reads the next byte of the input stream, applying some bodges.
 */
static int egg_getc()
{
   int c = do_getc();

   if ((c >= 'A') && (c <= 'Z'))
      c = c - 'A' + 'a';

   if (c == '#') {
      while ((c != '\n') && (c != EOF))
	 c = do_getc();
   }

   if (c == '\n')
      egg_line++;

   return c;
}



/* egg_ungetc:
 *  Pushes an unwanted character back into the input stream.
 */
static void egg_ungetc(int c)
{
   egg_unget_char = c;

   if (c == '\n')
      egg_line--;
}



/* egg_eof:
 *  Have we reached the end of the file?
 */
static int egg_eof()
{
   if (egg_file)
      return pack_feof(egg_file);
   else
      return (egg_data_length > 0);
}



/* iswordchar:
 *  Returns true if this character is valid as part of an identifier.
 */
static int iswordchar(int c)
{
   return (((c >= 'A') && (c <= 'Z')) ||
	   ((c >= 'a') && (c <= 'z')) ||
	   (c == '_'));
}



/* get_word:
 *  Reads a word from the input stream.
 */
static void get_word(char *buf)
{
   int c, i;

   for (;;) {
      c = egg_getc();

      if (c == EOF) {
	 buf[0] = 0;
	 return;
      }

      if (iswordchar(c)) {
	 i = 0;

	 for (;;) {
	    buf[i++] = c;

	    c = egg_getc();

	    if (!iswordchar(c)) {
	       egg_ungetc(c);
	       buf[i] = 0;
	       return;
	    }
	 }
      }

      if (!isspace(c)) {
	 buf[0] = c;
	 buf[1] = 0;
	 return;
      }
   }
}



/* get_formula:
 *  Reads a formula from the input stream, finishing when the specified
 *  terminator character is encountered.
 */
static void get_formula(char *buf, int terminator, char *error)
{
   int inspace = FALSE;
   int braces = 0;
   int i = 0;
   int c;

   for (;;) {
      c = egg_getc();

      if ((c == terminator) && (braces <= 0)) {
	 buf[i] = 0;
	 return;
      }

      if (c == EOF) {
	 egg_error(error, "Unexpected EOF");
	 return;
      }

      if (isspace(c)) {
	 if ((i > 0) && (!inspace)) {
	    buf[i++] = ' ';
	    inspace = TRUE;
	 }
      }
      else {
	 buf[i++] = c;

	 if (terminator == ')') {
	    if (c == '(')
	       braces++;
	    else if (c == ')')
	       braces--;
	 }
      }

      if (i >= 1023) {
	 egg_error(error, "Missing '%c'", terminator);
	 return;
      }
   }
}



/* get_brace:
 *  Reads a '{' from the input stream, giving an error if it is not found.
 */
static void get_brace(char *error)
{
   char buf[256];

   get_word(buf);

   if (strcmp(buf, "{") != 0)
      egg_error(error, "Missing '{'");
}



/* check_ascii_word:
 *  Checks that a string is a valid variable name.
 */
static void check_ascii_word(char *buf, char *error)
{
   int i;

   for (i=0; buf[i]; i++) {
      if (!iswordchar(buf[i]))
	 break;
   }

   if (i <= 0)
      egg_error(error, "Missing identifier");
   else if (buf[i])
      egg_error(error, "Invalid character ('%c') in identifier", buf[i]);
}



/* get_ascii_word:
 *  Reads a word and checks that it is a valid variable name.
 */
static void get_ascii_word(char *buf, char *error)
{
   get_word(buf);
   check_ascii_word(buf, error);
}



/* load_egg_cmd:
 *  Reads a list of commands.
 */
static EGG_COMMAND *load_egg_cmd(EGG *egg, int level, char *error)
{
   EGG_COMMAND *cmd = NULL;
   EGG_COMMAND *tail = NULL;
   EGG_TYPE *type;
   char buf[1024];
   char buf2[256];


   /* helper macro for inserting new commands into the list */
   #define ADD_COMMAND(_type_)                                 \
   {                                                           \
      if (tail) {                                              \
	 tail->next = malloc(sizeof(EGG_COMMAND));             \
	 tail = tail->next;                                    \
      }                                                        \
      else                                                     \
	 tail = cmd = malloc(sizeof(EGG_COMMAND));             \
							       \
      tail->type = _type_;                                     \
      tail->line = egg_line;                                   \
      tail->var = NULL;                                        \
      tail->exp = NULL;                                        \
      tail->cmd = NULL;                                        \
      tail->cmd2 = NULL;                                       \
      tail->next = NULL;                                       \
   }


   while ((!egg_eof()) && (!error[0])) {
      get_word(buf);

      if (strcmp(buf, "}") == 0) {
	 /* block end marker */
	 if (level==0)
	    egg_error(error, "Unexpected '}'");
	 else
	    return cmd;
      }
      else if (strcmp(buf, "if") == 0) {
	 /* parse an if statement */
	 get_word(buf);

	 if (strcmp(buf, "(") != 0) {
	    egg_error(error, "Missing '('");
	 }
	 else {
	    get_formula(buf, ')', error);

	    if (!error[0]) {
	       get_brace(error);

	       if (!error[0]) {
		  ADD_COMMAND(EGG_COMMAND_IF);

		  tail->exp = malloc(strlen(buf)+1);
		  strcpy(tail->exp, buf);

		  tail->cmd = load_egg_cmd(egg, level+1, error);
	       }
	    }
	 }
      }
      else if (strcmp(buf, "else") == 0) {
	 /* parse an else statement */
	 if ((!tail) || (tail->type != EGG_COMMAND_IF) || (tail->cmd2)) {
	    egg_error(error, "Invalid context for 'else'");
	 }
	 else {
	    get_brace(error);

	    if (!error[0])
	       tail->cmd2 = load_egg_cmd(egg, level+1, error);
	 }
      }
      else if (strcmp(buf, "while") == 0) {
	 /* parse a while statement */
	 get_word(buf);

	 if (strcmp(buf, "(") != 0) {
	    egg_error(error, "Missing '('");
	 }
	 else {
	    get_formula(buf, ')', error);

	    if (!error[0]) {
	       get_brace(error);

	       if (!error[0]) {
		  ADD_COMMAND(EGG_COMMAND_WHILE);

		  tail->exp = malloc(strlen(buf)+1);
		  strcpy(tail->exp, buf);

		  tail->cmd = load_egg_cmd(egg, level+1, error);
	       }
	    }
	 }
      }
      else if (strcmp(buf, "lay") == 0) {
	 /* parse a lay statement */
	 ADD_COMMAND(EGG_COMMAND_LAY);

	 get_word(buf);

	 if (strcmp(buf, "(") == 0) {
	    get_formula(buf, ')', error);

	    if (!error[0]) {
	       tail->exp = malloc(strlen(buf)+1);
	       strcpy(tail->exp, buf);

	       get_word(buf);
	    }
	 }

	 if (!error[0]) {
	    check_ascii_word(buf, error);

	    if (!error[0]) {
	       tail->var = malloc(strlen(buf)+1);
	       strcpy(tail->var, buf);

	       get_word(buf);

	       if (strcmp(buf, "{") == 0)
		  tail->cmd = load_egg_cmd(egg, level+1, error);
	       else if (strcmp(buf, ";") != 0)
		  egg_error(error, "Expecting '{' or ';'");
	    }
	 }
      }
      else if (strcmp(buf, "die") == 0) {
	 /* parse a die statement */
	 ADD_COMMAND(EGG_COMMAND_DIE);

	 get_word(buf);

	 if (strcmp(buf, ";") != 0)
	    egg_error(error, "Missing ';'");
      }
      else if (strcmp(buf, "srand") == 0) {
	 /* parse a srand statement */
	 ADD_COMMAND(EGG_COMMAND_SRAND);

         if (!error[0]) {
            tail->var = malloc(strlen(buf)+1);
	    strcpy(tail->var, buf);

	    get_formula(buf, ';', error);

	    if (!error[0]) {
	       tail->exp = malloc(strlen(buf)+1);
	       strcpy(tail->exp, buf);
	    }
         }
      }
      else if (strcmp(buf, "type") == 0) {
	 /* parse a type definition */
	 if (level) {
	    egg_error(error, "Nested type definition");
	 }
	 else {
	    get_ascii_word(buf, error);

	    if (!error[0]) {
	       type = malloc(sizeof(EGG_TYPE));

	       type->name = malloc(strlen(buf)+1);
	       strcpy(type->name, buf);

	       type->cmd = NULL;
	       type->next = NULL;

	       get_brace(error);

	       if (error[0]) {
		  free(type->name);
		  free(type);
	       }
	       else {
		  type->cmd = load_egg_cmd(egg, level+1, error);

		  if (error[0]) {
		     free(type->name);
		     free(type);
		  }
		  else {
		     type->next = egg->type;
		     egg->type = type;
		  }
	       }
	    }
	 }
      }
      else if (buf[0]) {
	 /* this must be a variable assignment */
	 check_ascii_word(buf, error);

	 if (!error[0]) {
	    get_word(buf2);

	    if (strcmp(buf2, "=") == 0) {
	       ADD_COMMAND(EGG_COMMAND_SET);
	    }
	    else if (strcmp(buf2, ":") == 0) {
               if (level==1) {
	          get_word(buf2);

	          if (strcmp(buf2, "=") != 0) {
		     egg_error(error, "Missing '='");
	          }
	          else {
		     ADD_COMMAND(EGG_COMMAND_INIT);
	          }
               }
               else {
                  egg_error(
                    error, 
                    "':=' initialization is not at top level "
                    "of particle declaration"
                  );
               }
	    }
	    else {
	       egg_error(error, "Fucked up syntax ");
	    }

	    if (!error[0]) {
	       tail->var = malloc(strlen(buf)+1);
	       strcpy(tail->var, buf);

	       get_formula(buf, ';', error);

	       if (!error[0]) {
		  tail->exp = malloc(strlen(buf)+1);
		  strcpy(tail->exp, buf);
	       }
	    }
	 }
      }
   }

   if ((!error[0]) && (egg_eof()) && (level != 0))
      egg_error(error, "Unexpected EOF");

   if (error[0]) {
      destroy_egg_cmd(cmd);
      return NULL;
   }

   return cmd;
}



/* do_load:
 *  Worker function for loading EGG scripts.
 */
static EGG *do_load(char *error)
{
   EGG *egg;
   EGG_COMMAND *cmd;

   egg_line = 1;

   egg_unget_char = 0;

   error[0] = 0;

   egg = malloc(sizeof(EGG));

   egg->frame = 0;
   egg->part_count = 0;
   egg->part_num = 0;
   egg->part = NULL;
   egg->type = NULL;
   egg->var = NULL;

   cmd = load_egg_cmd(egg, 0, error);

   if (error[0]) {
      destroy_egg(egg);
      return NULL;
   }

   process_egg_cmd(egg, NULL, NULL, cmd, TRUE, TRUE, error);

   destroy_egg_cmd(cmd);

   if (error[0]) {
      destroy_egg(egg);
      return NULL;
   }

   return egg;
}



/* load_egg:
 *  Reads an EGG script from a disk file.
 */
EGG *load_egg(char *filename, char *error)
{
   EGG *egg;

   egg_file = pack_fopen(filename, F_READ);

   if (!egg_file) {
      strcpy(error, "File not found");
      return NULL;
   }

   egg = do_load(error);

   pack_fclose(egg_file);
   egg_file = NULL;

   return egg;
}



/* use_egg:
 *  Reads an EGG script from a block of memory.
 */
EGG *use_egg(void *data, int length, char *error)
{
   EGG *egg;

   egg_data = data;
   egg_data_length = length;

   egg = do_load(error);

   egg_data = NULL;
   egg_data_length = 0;

   return egg;
}


