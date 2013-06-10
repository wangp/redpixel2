/* readline.c -- configuration module for lua.c; see config.c
*
* This module implements of lua_readline and lua_saveline for lua.c using
* the GNU readline and history libraries. It should also work with drop-in
* replacements such as editline and libedit (you may have to include different
* headers, though).
*
*/

#define lua_readline	myreadline
#define lua_saveline	mysaveline

#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

static void mysaveline (lua_State *L, const char *s) {
  const char *p;
  for (p=s; isspace(*p); p++)
    ;
  if (*p!=0) {
    size_t n=strlen(s)-1;
    if (s[n]!='\n')
      add_history(s);
    else {
      lua_pushlstring(L,s,n);
      s=lua_tostring(L,-1);
      add_history(s);
      lua_remove(L,-1);
    }
  }
}

static int myreadline (lua_State *L, const char *prompt) {
  char *s=readline(prompt);
  if (s==NULL)
    return 0;
  else {
    lua_pushstring(L,s);
    lua_pushliteral(L,"\n");
    lua_concat(L,2);
    free(s);
    return 1;
  }
}
