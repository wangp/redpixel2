/*
 * gui.js
 * The JavaScript module of JS GUI.
 */


/*
 * GUIWidget superclass
 */

function GUIWidget(parent, name)
{
    this.type = "GUIWidget";
    this.name = name || "(unnamed)";
    this.label = "";
    this.children = [ ];
    this.x = this.y = null;
    this.w = this.h = null;
    this.draw = null;
    this.parent = parent || null;
    
    if (parent)
      parent.insert_child(this);
    
    /* Layout code */
   
    this.layout_list = [ ];

    this.widget = function (name, type, a, b, c, d, e, f, g) {
	q = GUI.internal.new_widget(this, name, type, a, b, c, d, e, f, g);
	this.layout_list.push(name);
	return q;
    }

    this.br = function () {
	this.layout_list.push("<br>");
    }

    this.setup = function (set, sx, sy) {
	var count = set.length;
	var ww = this.w - 40 - (count-1) * 10;
	var hh = 20;		/* FIXME */

	for (var i in set) {
	    var str = set[i];
	    this.children[str].x = sx;
	    this.children[str].y = sy;
	    this.children[str].w = ww / count;
	    this.children[str].h = hh;
	    sx += this.children[str].w + 10;
	}
    }

    this.commit = function () {
	var arr = [ ], xpos = 20, ypos = 20;

	for (i in this.layout_list) {
	    str = this.layout_list[i];
	    if (str != "<br>") 
	      arr.push(str);
	    else {
		this.setup(arr, xpos, ypos);
		arr = [ ];
		ypos += 30;	/* FIXME */
	    }
	}

	if (arr.length)
	  this.setup(arr, xpos, ypos);
	this.layout_list = [ ];
    }
    
    /* Tree functions */
    
    this.insert_child = function (c) {
	this.children[c.name] = c;
	c.parent = this;
    }
    
    this.remove_child = function (c) {
	tmp = [ ];
	for (i in this.children) 
	  if (this.children[i] != c) 
	    tmp.push(this.children[i]);
	this.children = tmp;
	c.parent = null;
    }    

    this.draw_tree = function (bmp, x, y) {
	GUI.internal.draw_basic
	    (bmp, x + this.x, y + this.y);
	for (i in this.children) {
	    this.draw_tree(bmp, this.x + x, this.y + y);
	}
    }


    /* Signals and slots */
    
    this.sigslot_list = [ ];

    this.set_slot = function (signal, slot) { 
	this.sigslot_list[signal] = slot;
    }


    /*
     * End of the world!
     * Not to be confused with Y2K.
     */
    
    this.die = function () {
	if (this.parent) 
	  this.parent.remove_child(this);
    }


    /* 
     * debugging 
     */
    this.spew = function () {
	print("---------- hurl ---------\n");
	print("Name: ", this.name, "\n");
	print("\tX,Y: ", this.x, this.y, "\n");
	print("\tW,H: ", this.w, this.h, "\n");
    }
}


/*
 * GUIStubWidget
 */

function GUIStubWidget(parent, name)
{
    this.base = GUIWidget;
    this.base(parent, name);
    this.type = "GUIStubWidget";
}

GUIStubWidget.prototype = new GUIWidget;


/*
 * `GUI' Object
 */

var GUI = new function () {
    
    /* inherit from GUIWidget */
    this.base = GUIWidget;
    this.base();
    
    this.type = "GUI";
    this.name = "GUI";

    /* Root property */
    this.Root = new GUIWidget();
    this.Root.type = "Root";
    this.Root.name = "GUI.Root";
    this.Root.w = 800;
    this.Root.h = 600;

    this.Root.window = function (name, label, w, h, x, y) {
	win = this.widget(name, "window", label, w, h, x, y);
	return win;
    }

    /* internal property */

    this.internal = new function () { /* nothing */ };

    this.internal.new_widget = function (parent, name, type, a, b, c, d, e, f, g) {
	switch (type) {
	    case "pushbutton":
	    case "textbox":
	    case "label":
		return new GUIStubWidget(parent, name, a, b, c, d, e, f, g);
	    
	    case "window": 
		/* a: label, b: w, c: h, d: x, e: y */
		g = new GUIStubWidget(parent, name, a, b, c, d, e, f, g);
		id = GUI.internal.new_client(g, b, c);
		g.id = id;
		return g;
		
	    default:
		return null;
	}
    }
}

GUI.prototype = new GUIWidget;
