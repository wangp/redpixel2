/* base.js
 *
 * Peter Wang <tjaden@psynet.net>
 */


/* BaseArray:
 *  Linear list of all available base tiles (string format).
 *  NOT a widget (see basepalette)
 */

function BaseArray() {
    this.list = [];
    GUI.__native.make_base_array(this.list);
    this.list.sort();
    
    this.index = function (name) {
	len = this.list.length;
	for (i=0; i<len; i++) 
	    if (this.list[i] == name)
		return i;
	return undefined;
    }
}



/* base_palette: 
 *  Showing and selecting the list of tiles. (widget)
 */

function __rpxedit__basepalette(a, b) {
    this.gridw = a;
    this.gridh = b;
    this.top = 0;
    this.selected = 0;
    this.draw = GUI.__native.draw_basepalette;

    this.button_up = function (mb, mx, my) {
	w = parseInt(this.w / this.gridw);
	h = parseInt(this.h / this.gridh);
	x = parseInt(mx / this.gridw);
	y = parseInt(my / this.gridh);
	
	if (x < w && y < h) {
	    this.selected = (this.top+y)*w + x;
	    this.signal("clicked", this.selected);
	    this.update();
	}
    }
}

GUI.new_widget_type("basepalette", __rpxedit__basepalette);



/* popup_base_palette:
 *  Window wrapping for BaseArray.
 */

var base_palette;

function change_base_selected(val) {
    base_selected = val;
    if (locked("basetiles")) {
	base_palette.selected = base_array.index(val);
	base_palette.update();
    }
    
    change_editing_mode("base");
}

function popup_base_palette() {
    if (!lock("basetiles"))
	return;
    
    bs = GUI.Root.window("basetiles", 255, 285, "Base tiles");
    pal = bs.widget("basepalette", "pal", 32, 32);
    sc = bs.widget("vscrollbar", null);
    bs.br();
    block = bs.widget("pushbutton", null, "Block " + (base_blocking ? "(on) " : "(off)"));
    hide = bs.widget("pushbutton", null, "Hide dialog");
    bs.commit();
    
    pal.src = base_array.list;
    base_palette = pal;
    
    x = parseInt(pal.w / pal.gridw);
    y = parseInt(pal.h / pal.gridh);
    tmp = parseInt(base_array.list.length / x);
    sc.max = tmp - y;
    
    pal.connect("clicked", pal, 
		function (val) {
		    base_selected = pal.src[val];
		    change_editing_mode("base");
		});
        
    sc.connect("value changed", pal, 
	       function (val) {
		   this.top = val;
		   this.update();
	       });
    
    block.connect("clicked", block,
		  function () {
		      base_blocking = !base_blocking;
		      if (base_blocking)
			  this.label = "Block (on) ";
		      else
			  this.label = "Block (off)";
		      this.update();
		  });
    
    hide.connect("clicked", bs,
		 function () {
		     this.kill(); 
		     base_palette_popped = 0;
		     unlock("basetiles");
		 });

    change_editing_mode("base");
}



/* 
 * Globals
 */

var base_array = new BaseArray();
var base_selected = base_array.list[0];
var base_blocking = 0;

