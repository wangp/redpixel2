/* themap.js
 * 
 * Peter Wang <tjaden@psynet.net>
 */


/* TheMap:
 *  The working space for map editing.  
 *  This is NOT a widget (see mapviewport)
 */

function TheMap(w, h) {
    this.w = w;
    this.h = h;
        
    this.base = [];
    for (y=0; y<h; y++)
	this.base[y] = [];

    this.base_drawline = 0;
    
    
    /* base editing */
    
    this.base_down = function (mb, mx, my) {
	tx = parseInt(mx / 16);
	ty = parseInt(my / 16);
	    
	if (mb == mouse.Middle) {
	    change_base_selected(this.base[ty][tx]);
	    return;
	}
	
	if (base_blocking) {
	    this.base_x1 = this.base_x2 = tx;
	    this.base_y1 = this.base_y2 = ty;
	    this.base_drawline = 1;
	}	
    }

    this.base_up = function (mb, mx, my) {
    	if (mb == mouse.Middle) 
	    return;
	    	
	if (base_blocking) {
	    tx = parseInt(mx / 16);
	    ty = parseInt(my / 16);
	    
	    this.base_x2 = tx;
	    this.base_y2 = ty;
	    this.base_drawline = 0;
	    
	    /* yuck */
	    if (this.base_x2 < this.base_x1) { 
		c = this.base_x2; 
		this.base_x2 = this.base_x1; 
		this.base_x1 = c; 
	    }
	    if (this.base_y2 < this.base_y1) {
		c = this.base_y2;
		this.base_y2 = this.base_y1; 
		this.base_y1 = c; 
	    }
	    
	    c = (mb == 0) ? base_selected : undefined;
	    
	    for (y=this.base_y1; y<=this.base_y2; y++) 
		for (x=this.base_x1; x<=this.base_x2; x++) 
		    this.base[y][x] = c;
	}	
    }
    
    this.base_hold = function (mb, mx, my) {
	if (mb == mouse.Middle) 
	    return;

	tx = parseInt(mx / 16);
	ty = parseInt(my / 16);
	    
	if (!base_blocking) {
	    this.base[ty][tx] = (mb == 0) ? base_selected : undefined;
	}
	else {
	    this.base_x2 = tx;
	    this.base_y2 = ty;
	}
    }
    
    
    /* prefab placing */
    
    this.prefab_down = function (mb, mx, my) {
	tx = parseInt(mx / 16);
	ty = parseInt(my / 16);
	
	if (mb == mouse.Right) {
	    this.base[ty][tx] = undefined;
	    return;
	}

	i = cache_lookup("prefab_palette_selected", 0);
	pref = prefabs.list[i];
	
	for (y=0; y<pref.h; y++)
	    for (x=0; x<pref.w; x++)
		this.base[ty+y][tx+x] = pref.table[y][x];
    }    
    
    
    /* "editting" */
    
    this.down = function (mb, mx, my) {
	func = __rpxedit__editing_mode + "_down";
	if (this[func])
	    this[func](mb, mx, my);
    }
    
    this.up = function (mb, mx, my) {
	func = __rpxedit__editing_mode + "_up";
	if (this[func])
	    this[func](mb, mx, my);
    }
    
    this.hold = function (mb, mx, my) {
	func = __rpxedit__editing_mode + "_hold";
	if (this[func])
	    this[func](mb, mx, my);
    }
}



/* mapviewport:
 *  The associated widget for TheMap.
 */

function mapviewport(a, b) {
    this.scroll_x = 0;
    this.scroll_y = 0;
    
    this.draw = GUI.__native.draw_mapviewport;
    
    this.check_ok = function (mx, my) {
	return ((mx<0) || (mx>=this.w) || (my<0) || (my>=this.h)) ? 0 : 1;
    }

    this.do_signal = function (signal, mb, mx, my) {
	this.signal(signal, mb, mx+this.scroll_x*16, my+this.scroll_y*16);
    }

    this.button_down = function (mb, mx, my) {
	if (this.check_ok(mx, my))
	    this.do_signal("button down", mb, mx, my);
    }
    
    this.button_hold = function (mb, mx, my) {
	if (this.check_ok(mx, my))
	    this.do_signal("button hold", mb, mx, my);
    }

    this.button_up = function (mb, mx, my) {
	if (this.check_ok(mx, my))
	    this.do_signal("button up", mb, mx, my);
    }
}



/* Canvas:
 *  Window wrapping for `mapviewport'.
 */

function popup_canvas() {
    
    cv = GUI.Root.window(null, .6, .7, "Canvas");
    xypos = cv.widget("label", null, "", GUI.label.Left);
    size = cv.widget("label", null, "", GUI.label.Right);
    cv.br();
    viewport = cv.widget("mapviewport", null);
    vscroll = cv.widget("vscrollbar");
    cv.br();
    hscroll = cv.widget("hscrollbar");
    cv.commit();
    
    viewport.src = the_map;
    viewport.connect("button down", viewport,
		     function (b, x, y) {
			 the_map.down(b, x, y);
			 this.update();
		     });
    viewport.connect("button hold", viewport,
		     function (b, x, y) {
			 the_map.hold(b, x, y);
			 this.update();
		     });
    viewport.connect("button up", viewport,
		     function (b, x, y) {
			 the_map.up(b, x, y);
			 this.update();
		     });
    
    function mkposlabel(x, y) {
	return "Pos: (" + viewport.scroll_x + ", " + viewport.scroll_y + ")";
    }

    vscroll.max = the_map.h;
    vscroll.connect("value changed", viewport,
		    function (val) {
			this.scroll_y = val;
			xypos.label = mkposlabel(this.scroll_x, this.scroll_y);
			this.update();
		    });

    hscroll.max = the_map.w;
    hscroll.connect("value changed", viewport,
		    function (val) {
			this.scroll_x = val;
			xypos.label = mkposlabel(this.scroll_x, this.scroll_y);
			this.update();
		    });
    
    xypos.label = mkposlabel(viewport.scroll_x, viewport.scroll_y);
    size.label = "Size: (" + the_map.w + ", " + the_map.h + ")";
}



/*
 *  main / globals
 */

GUI.new_widget_type("mapviewport", mapviewport);

var the_map = new TheMap(128, 128);

