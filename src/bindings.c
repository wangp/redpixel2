/* bindings.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "bindings.h"


void __bindings_object_export_functions ();
void __bindings_store_export_functions ();


void bindings_init ()
{
    __bindings_object_export_functions ();
    __bindings_store_export_functions ();
}


void bindings_shutdown ()
{
}
