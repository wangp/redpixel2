/*
 * jsgui.js - the JS portion of our API
 */


/*
 * Widget Superclass
 */

function __gui_Widget(label, parent) {

    this.type = "(widget)";
    this.label = label || "";
    this.parent = parent || null;
    this.children = [ ];
    this.x = this.y =
      this.w = this.h = undefined;
    this.draw = GUIRoot.__internal.DrawPushButton;

    if (parent)
      parent.insert_child(this);

    /*
     * Layout code.
     */
    
    this.__child_list = [ ];
    this.__layout_list = [ ];

    this.widget = function (name, type, label) {
	this.children[name] = 
	  new GUIRoot.__internal.new_widget(type, label, this);
	this.__layout_list.push(name);
    }

    this.br = function (size) {
	this.__layout_list.push("~br");
    }

    this.__setup_widget_set = function (set, sx, sy) {
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
	var arr = [ ];
	var xx = 20;
	var yy = 20;

	for (var i in this.__layout_list) {
	    var str = this.__layout_list[i];

	    if (str != "~br") {
		arr.push(str);
	    }
	    else {
		this.__setup_widget_set(arr, xx, yy);
		arr = [ ];
		yy += 30;	/* FIXME */
	    }
	}

	if (arr.length) 
	  this.__setup_widget_set(arr, xx, yy);
	this.__layout_list = [ ];
    }

    
    /*
     * Family tree.
     */
    
    this.insert_child = function (widget) {
	this.__child_list.push(widget);
	this.children[widget] = widget;
    }

    this.remove_child = function (widget) {
	var arr = [ ];
	for (var i in this.__child_list) {
	    if (this.__child_list[i] != widget) {
		arr.push(this.__child_list[i]);
	    }
	}
	this.__child_list = arr;
    }
    
    this.__draw_self_and_children = function (dest, px, py) {
	this.draw(dest, px + this.x, py + this.y);
	for (var ch in children) {
	    ch.__draw_self_and_children(dest, px + this.x, py + this.y);
	}
    }
    
    this.update = function () {
	  this.parent.update();
    }
    
    
    /*
     * Signals and slots.
     */
    
    this.__sigslot = [ ];

    this.set_slot = function (signal, slot) { 
	this.__sigslot[signal] = slot;
    }
    

    /*
     * End of the world!
     * Not to be confused with Y2K.
     */
    
    this.die = function () {
	if (this.parent) {
	    this.parent.remove_child(this);
	}
    }
}



/*
 * __gui_Window 
 */

function __gui_Window(label, parent, w, h) {
    this.base = __gui_Widget;
    this.base(label);		       /* don't have widget parents */
    this.type = "Window";
    
    this.update = function () {
	GUIRoot.__internal.mark_client(this.id);
    }
}

__gui_Window.prototype = new __gui_Widget;



/*
 * __gui_PushButton
 */

function __gui_PushButton(label, parent) {
    this.base = __gui_Widget;
    this.base(label, parent);
    this.type = "PushButton";
}

__gui_PushButton.prototype = new __gui_Widget;



/*
 * __gui_Label
 */

function __gui_Label(label, parent) {
    this.base = __gui_Widget;
    this.base(label, parent);
    this.type = "Label";
}

__gui_Label.prototype = new __gui_Widget;



/*
 * __gui_TextBox
 */

function __gui_TextBox(label, parent, w, h) {
    this.base = __gui_Widget;
    this.base(label, parent);
    this.type = "TextBox";
}

__gui_TextBox.prototype = new __gui_Widget;



/*
 * GUIRoot (``supplemental'' to C code)
 */


/*
 * Our hard-working translator
 */

GUIRoot.__internal.new_widget = function (type, label, name) {
    switch (type) {
      case "Widget":		return new __gui_Widget(label, name);
      case "Window":		return new __gui_Window(label, name);
      case "PushButton":	return new __gui_PushButton(label, name);
      case "Label":		return new __gui_Label(label, name);
      case "TextBox":		return new __gui_TextBox(label, name);
      default: 			return null;
    }
}


/*
 * Pop up a new window
 */

GUIRoot.children = [ ];

GUIRoot.window = function (name, w, h, label) {
    if (w < 1) w *= GUIRoot.w;
    if (h < 1) h *= GUIRoot.h;

    var win = GUIRoot.__internal.new_widget("Window", label, name);
    var id = GUIRoot.__internal.new_client(win, w, h);

    GUIRoot.children.push(win);
    win.x = win.y = 0;		       /* handled by wm */
    win.w = w;		       	       /* for read-only purposes */
    win.h = h;
    win.id = id;		       /* FIXME: still required? */
    win.update();
}

