/* export.c : export functions / globals to SeeR scripts */

#include <stdio.h>		       /* printf is exported */
#include <allegro.h>
#include <seer.h>

#include "exalleg.h"

/* headers with exports */
#include "mapedit.h"
#include "script.h"
#include "tiles.h"
#include "wrapper.h"
#include "weapon.h"
#include "object.h"

void export()
{
    /* export allegro stuff (exallegro.c) */
    export_allegro();
    
    
    /*
     * cheat and use the auto generated file _export.c :)
     * ugly but *so* much easier !
     */
    #include "_export.c"
}

