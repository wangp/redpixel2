/* prefab.js - prefab bases
 * 
 * Peter Wang <tjaden@psynet.net>
 */


function __rpxedit__Prefab(name) {
    this.name = name || "(unnamed)"
    this.table = [];
    this.w = this.h = 0;
}


function PrefabArray() {
    
    this.list = [];
    self = this;
    
    /* just init these */
    row_num = undefined;
    cur_row = undefined;
    prefab = undefined;
    
    function elem_start(name, atts) {
	if (name == "prefab") {
	    prefab = new __rpxedit__Prefab(atts["name"]);
	    self.list.push(prefab);
	    row_num = 0;
	}
	if (name == "row") {
	    cur_row = prefab.table[row_num] = [];
	}
	else if (name == "tile") {
	    cur_row.push(atts["name"]);
	}
	else if (name == "notile") {
	    cur_row.push(null);
	}
    }
    
    function elem_end(name) {
	if (name == "row") {
	    w = cur_row.length;
	    if (w > prefab.w)
		prefab.w = w;
	    
	    row_num++;
	}
	else if (name == "prefab") {
	    prefab.h = prefab.table.length;
	}
    }
    
    XML.set_element_handler(elem_start, elem_end);
    XML.set_character_data_handler(null);
    XML.parse("data/tiles/prefabs.xml");
} 



/* prefab_palette:
 *  Display and allow selecting of the list.
 *  (based on gridpalette)
 */

function __rpxedit__prefabpalette(a, b) {
    this.gridw = a;
    this.gridh = b;
    this.top = 0;
    this.selected = 0;
    this.draw = GUI.__native.draw_prefabpalette;
    
    this.button_up = function (mb, mx, my) {
	w = parseInt(this.w / this.gridw);
	h = parseInt(this.h / this.gridh);
	x = parseInt(mx / this.gridw);
	y = parseInt(my / this.gridh);
	
	if (x < w && y < h) {
	    this.selected = (this.top+y)*w + x;
	    this.signal("clicked", this.selected);
	    this.update();
	    change_editing_mode("prefab");
	}
    }
}

GUI.new_widget_type("prefabpalette", __rpxedit__prefabpalette);



/* popup_prefab_palette:
 *  Window dressing.
 */

function popup_prefab_palette() {
    if (!lock("Prefabs"))
	return;
    
    win = GUI.Root.window("prefabs", 290, 270, "Prefab'd bases");
    win.widget("label", "name", "(name)");
    win.widget("label", "size", "2x2", GUI.label.Right);
    win.br();
    palette = win.widget("prefabpalette", "pal", 64, 64);
    vscroll = win.widget("vscrollbar", null);
    win.br();
    hide = win.widget("pushbutton", null, "Hide");
    win.commit();
    
    /* down to business */
    
    win.set_labels = function (sel) {
	p = prefabs.list[sel];
	if (p) {
	    this.children.name.label = p.name;
	    this.children.size.label = p.w + "x" + p.h;
	    this.children.name.update();
	    this.children.size.update();
	}
    }
    
    palette.src = prefabs;
    palette.selected = cache_lookup("prefab_palette_selected", 0);
    win.set_labels(palette.selected);
    palette.connect("clicked", win,
		    function (selection) {
			cache_value("prefab_palette_selected", selection);
			win.set_labels(selection);
		    });
    
    ww = parseInt(palette.w / palette.gridw);
    hh = parseInt(palette.h / palette.gridh);
    
    vscroll.max = prefabs.list.length/ww - hh + 1;
    if (vscroll.max < vscroll.min)
	vscroll.max = vscroll.min;
    print(vscroll.max, "\n");
    vscroll.connect("value changed", palette, 
		    function (val) {
			this.top = val;
			this.update();
		    });
    
    hide.connect("clicked", win,
		 function () {
		     win.kill();
		     unlock("Prefabs");
		 });

    change_editing_mode("prefab");
}



/*
 *  Global
 */

prefabs = new PrefabArray("data/tiles/prefabs.xml");


