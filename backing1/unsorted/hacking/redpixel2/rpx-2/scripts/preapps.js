/* preapps.js - think "prefab"
 *
 * Peter Wang <tjaden@psynet.net>
 */


function notyet(info) {
    n = GUI.Root.window(null, 250, 100, "Sorry...");
    n.widget("label", null, "Not implemented yet.", GUI.label.Centre);
    n.br();
    if (info) {
	n.widget("label", null, info, GUI.label.Centre);
	n.br();
    }
    ok = n.widget("pushbutton", "ok", "Ok");
    n.commit();
    
    ok.connect("clicked", n, 
	       function () { 
		   this.kill();
	       });
}



