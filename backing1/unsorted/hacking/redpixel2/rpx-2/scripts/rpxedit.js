/* rpxedit.js
 *
 * Peter Wang <tjaden@psynet.net>
 */


load("scripts/lock.js");
load("scripts/cache.js");
load("scripts/widgets.js");
load("scripts/preapps.js");

load("scripts/prefab.js");
load("scripts/base.js");
load("scripts/themap.js");



mouse = [];
mouse.Left = 0;
mouse.Right = 1;
mouse.Middle = 2;



/* 
 * editing mode
 */

var __rpxedit__editing_mode = "tell you later";

function change_editing_mode(newmode) {
    if (__rpxedit__editing_mode == newmode)
	return;
    
    main = cache_lookup("main window", null);
    __rpxedit__editing_mode = 
	main.children.modelabel.label = newmode;
    main.children.modelabel.update();
}



/*
 * main
 */

do {
    main = GUI.Root.window("main", 250, 100, "Main");
    main.br();
    base = main.widget("pushbutton", "basebutton", "Bases");
    prefab = main.widget("pushbutton", null, "Prefab");
    main.widget("pushbutton", null, "null");
    main.widget("pushbutton", null, "null");
    main.br();
    main.widget("pushbutton", null, "nil");
    main.widget("pushbutton", null, "nil");
    main.widget("pushbutton", null, "null");
    quit = main.widget("pushbutton", "quitbutton", "Quit");
    main.br();
    main.widget("label", null, "Editing mode:", GUI.label.Right);
    main.widget("label", "modelabel", "(touch me there)", GUI.label.Left);
    main.commit();
    
    cache_value("main window", main);
    change_editing_mode("base");
    
    base.connect("clicked", main, function() { popup_base_palette() });
    prefab.connect("clicked", main, function() { popup_prefab_palette() });
    
    quit.connect("clicked", main,
		 function () {
		     notyet("Just push escape for now.");
		 });

    /* bring up the canvas */
    popup_canvas();
    
} while (0);

