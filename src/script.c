/* script.c : SeeR script related functions */

#include <stdlib.h>
#include <seer.h>
#include "report.h"


typedef struct script_file 
{
    scScript sc;
    scInstance *prog;    
    struct script_file *prev, *next;    
} script_file_t;

static script_file_t *head = NULL;


/* addressof:
 *  Returns the address of a symbol in the current running
 *  instance.  Designed to be used inside a function.
 */
int addressof(char *symbol)
{
    return scGet_Symbol(scActual_Instance, symbol);
}


/* script_error:
 *  Outputs SeeR error strings.
 */
static void script_error(void)
{
    errprintf("%s\n> %s\n", scErrorMsg, scErrorLine);
}


/* exec_script:
 *  Runs a script, calling function FUNC.
 */
int exec_script(char *filename, char *func)
{
    scScript sc;
    scInstance *prog;
    int ret = 0;
    
    consoleprintf("Executing script %s\n", filename);
    
    sc = scCompile_File(filename);
    if (!sc) {
	errprintf("Error compiling %s!\n", filename);
	script_error();
	return -1;
    }
    
    prog = scCreate_Instance(sc, "");
    if (!prog) {
	errprintf("Error creating instance of %s!\n", filename);
	script_error();
	free(sc);
	return -1;
    } 
    
    scCall_Instance(prog, scGet_Symbol(prog, func));
    if (scErrorNo) {
	errprintf("Error calling `%s' in %s!\n", func, filename);
	//script_error();
	ret = -1;
    }
    
    scFree_Instance(prog);
    free(sc);
    return ret;
}



/* add_script:
 *  Compiles script then executes `script_main' (if it exists).
 *  Adds it to an internal table to be freed at the end of the program.
 *  Returns zero on success, non-zero on error.
 */

static void add(script_file_t *sf)
{
    if (!head) {
	head = sf;
	sf->next = sf->prev = NULL;
    } else {
	script_file_t *it = head;
	while (it->next)
	  it = it->next;
	it->next = sf;
	sf->next = NULL;
	sf->prev = it;
    }
}

int add_script(char *filename)
{
    script_file_t *sf;
    
    sf =  malloc(sizeof(script_file_t));
    if (!sf) {
	errprintf("Out of memory in `add_script'... WHAT?!\n");
	return -1;
    }
    
    consoleprintf("Adding script %s\n", filename);
    
    sf->sc = scCompile_File(filename);
    if (!sf->sc) {
	errprintf("Error compiling %s!\n", filename);
	script_error();
	return -1;
    }
    
    sf->prog = scCreate_Instance(sf->sc, "");
    if (!sf->prog) {
	errprintf("Error creating instance of %s!\n", filename);
	script_error();
	free(sf->sc);
	return -1;
    }
    
    add(sf);

    scCall_Instance(sf->prog, scGet_Symbol(sf->prog, "script_main"));
    if (scErrorNo) {
	errprintf("Error calling `script_main' in %s!\n", filename);
	//script_error();
	//return -1;
    }

    return 0;
}


/* free_scripts:
 *  Frees internal table, calling `script_exit' for each entry.
 */

void free_scripts(void)
{
    script_file_t *it = head, *tmp;

    while (it) {
	scCall_Instance(it->prog, scGet_Symbol(it->prog, "script_exit"));

	scFree_Instance(it->prog);
	free(it->sc);

	tmp = it->next;
	free(it);	
	it = tmp;
    }
    
    head = NULL;
}
