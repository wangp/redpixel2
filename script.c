/* script.c : misc SeeR script related functions */

#include <stdlib.h>
#include <seer.h>
#include "report.h"


/* script_error:
 *  Outputs SeeR error strings.
 */
void script_error(void)
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
    
    sc = scCompile_File(filename);
    if (!sc) {
	errprintf("Error compiling %s!\n", filename);
	script_error();
	return 0;
    }
    
    prog = scCreate_Instance(sc, "");
    if (!prog) {
	errprintf("Error creating instance of %s!\n", filename);
	script_error();
	free(sc);
	return 0;
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

