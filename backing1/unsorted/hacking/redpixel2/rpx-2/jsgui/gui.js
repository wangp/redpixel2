/*
 * gui.js	-*- mode: C -*-
 * 
 * Peter Wang <tjaden@psynet.net>
 * 
 * If something starts with `__', most probably you aren't 
 * supposed to touch it.  But no one's stopping you.
 */


/*
 * Widget Superclass.
 * User programs shouldn't touch anything in here except:
 * 	widget, br, commit
 */
function __GUI_internal__WidgetBase() {
    
    this.x = 0;
    this.y = 0;
    this.w = 0;
    this.h = 0;

    this.children = [ ];
    this.parent = null;
    this.draw = null;
    this.resw = this.resh = 0;
    this.toplevel = false;
    
    
    /*
     * Tree functions
     */
    
    this.insert_child = function (c, name) {
	if (name)
	    this.children[name] = c;
	else
	    this.children.push(c);
	c.parent = this;
    }
    
    this.remove_child = function (c) {
	for (i in this.children) 
	    if (this.children[i] == c) {
		delete this.children[i];
		break;
	    }
	c.parent = null;
    }
    
    this.draw_tree = function (x, y) {
	if (this.draw)
	    this.draw(x+this.x, y+this.y);
	for (i in this.children) 
	    this.children[i].draw_tree(x+this.x, y+this.y);
    }
    
    this.focus_object = function (mx, my, x, y) {
	
	if ((mx >= x+this.x) && (mx < x+this.x+this.w) && 
	    (my >= y+this.y) && (my < y+this.y+this.h)) {
	    
	    for (i in this.children) {
		q = this.children[i].focus_object(mx, my, x+this.x, y+this.y);
		if (q) return q;
	    }

	    this.__rel_x = mx - (x+this.x);
	    this.__rel_y = my - (y+this.y);
	    
	    return this;
	}
	
	return null;	
    }
    
    this.update = function () {
	if (!this.toplevel)
	    this.parent.update();
	else
	    GUI.__native.mark_client(this.id);
    }
    
    this.kill = function () {
	if (!this.toplevel)
	    this.parent.kill();
	else 
	    GUI.__native.remove_client(this.id);
    }
    
    
    /* 
     * ``Initialising'' functions
     */
    
    this.wqueue = new Array(new Array());
    this.wcur = 0;

    this.widget = function (type, name, a, b, c) {   /* user */
	if (!__GUI_internal__WidgetBin[type])
	    return null;
	
	w = new __GUI_internal__WidgetBase();
	w.name = name;
	w.__tmp = __GUI_internal__WidgetBin[type];
	w.__tmp(a, b, c);
	delete w.__tmp;
	
	this.wqueue[this.wcur].push(w);
	
	return w;
    }
    
    this.br = function () {	       /* user */
	this.wcur++;
	this.wqueue[this.wcur] = new Array();
    }	
    
    
    /* Layout allocation function.
     * 
     * The theory:
     * 1. Allocate row sizes by searching through list and finding 
     *    reserved heights, if any.  Else allocate evenly.
     * 2. Go through each row, doing the same for columns (but not
     *    not taking into consideration other rows).  The widgets
     *    that want a specific width get it, unless there is not
     *    enough space, at which time they will be shrinked.  The
     *    rest divide up the scraps.
     * 
     * This function is WAY too big.  This is also the first time
     * I've written a layout thing like this, so it's probably
     * really inefficient (too many passes).
     */
    
    this.commit = function () {

	divider = 4;		       /* space to leave between */

	/* 
	 * Part I: allocate row sizes 
	 */
	
	rowsize = new Array();
	leftovers = dishout = this.h - 20 - (divider*2);
	unalloc = 0;
	
	/* first choice to reserved spacings */
	for (j in this.wqueue) {
	    
	    max = 0;
	    
	    for (i in this.wqueue[j]) {
		q = this.wqueue[j][i];
		if (q.resh < 1) 
		    hh = q.resh * dishout;
		else 
		    hh = q.resh;
		if (hh > max)
		    max = hh;
	    }
	    
	    if (max == 0)
		unalloc++;
	    rowsize[j] = max;
	    leftovers -= (max + divider);
	}
	
	leftovers += divider;
	
	/* scraps for the rest */
	if (unalloc) 
	    for (j in this.wqueue)
		if (rowsize[j] == 0) 
		    rowsize[j] = parseInt(leftovers / unalloc);
	
	
	/* 
	 * Part II: allocate widget widths for each row
	 */
	
	for (j in this.wqueue) {

	    leftovers = dishout = this.w - (divider*2);
	    unalloc = 0;
	    hh = rowsize[j];
	    
	    /* again, first choice to reservations */
	    for (i in this.wqueue[j]) {
		q = this.wqueue[j][i];
		
		if (q.resw && q.resw < 1)
		    ww = parseInt(q.resw * dishout);
		else 
		    ww = q.resw;
		
		if (!ww) unalloc++;
		leftovers -= (ww + divider);

		q.w = ww;
		q.h = hh;	       /* while we're at it */
	    }
	    
	    leftovers += divider;
	    
	    /* clean up the rest */
	    for (j in this.wqueue)
		for (i in this.wqueue[j])
		    with (this.wqueue[j][i]) 
			(w == 0) && (w = parseInt(leftovers / unalloc));
	}
	
		
	/* 
	 * The final stretch
	 */

	yy = 20 + divider + 1;
	for (j in this.wqueue) {
	    
	    xx = divider;
	    for (i in this.wqueue[j]) {
		q = this.wqueue[j][i];
		q.x = xx;
		q.y = yy;
		this.insert_child(q, q.name);

		xx += q.w + divider;
	    }
    
	    yy += rowsize[j] + divider;
	}

	
	/* reset */
	this.wqueue = new Array(new Array());
	this.wcur = 0;
    }
    
    
    /*
     * Signals & Slots
     */

    this.conn = [ ];
    
    this.connect = function (signal, recv, slot) {
	q = this;
	this.conn[signal] = new function () {
	    this.receiver = recv || q;
	    this.slot = slot;
	}
    }
    
    this.signal = function (signal, a, b, c) {
	q = this.conn[signal];
	if (q) {
	    q.receiver.__tmp = q.slot;
	    q.receiver.__tmp(a, b, c);
	}
    }
}



/*
 * Widget Bin
 * The *huge* collection of widgets up for use.
 */

var __GUI_internal__WidgetBin = new function () {

    /* __window:
     *  to be used by GUI.Root only 
     */
    this.__window = function () {
	this.base = __GUI_internal__WidgetBase;
	this.base();
	this.draw = GUI.__native.draw_window;
	this.toplevel = true;
	this.iconified = 0;
	
	this.button_down = function (mb, mx, my) {
	    if ((my <= 20) && (!this.iconified)) {
		if (mx > (this.w-20)) {
		    GUI.__native.client_special(this.id, GUI.wm.Shade);
		    return;
		} 
		else if (mx > (this.w-40)) {
		    GUI.__native.client_special(this.id, GUI.wm.Iconify);
		    this.iconified = 1;
		    this.allow_uniconify = 0;
		    return;
		}
	    }

	    this.offset_x = mx;
	    this.offset_y = my;
	    this.drag = 1;
	}
	
	this.button_hold = function (mb, mx, my) {
	    if (!this.drag)
		return;
	    
	    if (this.offset_x != mx ||
		this.offset_y != my) {
		
		GUI.__native.move_client(this.id, mx-this.offset_x, my-this.offset_y);
		this.offset_x = mx;
		this.offset_y = my;
		this.update();

		this.allow_uniconify = 0;
	    }
	}
	
	this.button_up = function () {
	    if (this.iconified && this.allow_uniconify) {
		this.iconified = 0;
		GUI.__native.client_special(this.id, GUI.wm.Iconify);    /* un- */
	    }

	    this.allow_uniconify = 1;
	    this.drag = 0;
	}
    }
    
    /* 
     * blank (dummy widget) 
     */
    this.blank = function (resw, resh) {
	this.resw = resw;
	this.resh = resh
    }
    
    /* 
     * pushbutton widget 
     */
    this.pushbutton = function (label) {
	this.draw = GUI.__native.draw_pushbutton;
	this.label = label || "";
	this.resh = 20;
	
	this.button_up = function (mb, mx, my) {
	    this.signal("clicked", mb, mx, my);
	}
    }
    
    /* 
     * label widget 
     */
    this.label = function (label, align) {
	this.draw = GUI.__native.draw_label;
	this.label = label || "(empty)";
	this.align = align || GUI.label.Left;
	this.resh = 20;
    }
    
    /* 
     * textbox widget 
     * (FIXME: unfinished) 
     */
    this.textbox = function (lines, text) {
	this.draw = GUI.__native.draw_filler;
	this.lines = lines || 5;
	this.text = text || "";
	this.resw = 0.9;
    }
}



/*
 * `GUI' Object
 * 
 * This is the central interface that user programs will see; almost 
 * everything else is for internal use only.
 * 
 * This is also that place the JSGUI will plug in callable C functions.
 * A central place (in __native) keeps namespace pollution to a minimum, 
 * not to mention being much easier to code. :)
 */

var GUI = new function () {
    
    /* nothing yet: this is filled with native C functions by JSGUI.
     * `native' was taken by JS itself (it seems), so we settle for this.
     */ 
    this.__native = new function () { /* ... */ }

    
    /* 
     * `Root' property 
     */
    
    this.Root = new __GUI_internal__WidgetBase();
    this.Root.w = 0;	
    this.Root.h = 0;
    
    this.__init_wh = function (w, h) {
	GUI.Root.w = w;
	GUI.Root.h = h;
    }
    
    this.Root.window = function (name, w, h, label) {
	if (name)
	    this.children[name] = (f = new __GUI_internal__WidgetBin.__window());
	else
	    this.children.push(f = new __GUI_internal__WidgetBin.__window());
	
	f.name = name;
	f.x = 0;
	f.y = 0;
	if (w < 1) f.w = parseInt(w * this.w); else f.w = w;
	if (h < 1) f.h = parseInt(h * this.h); else f.h = h;
	f.label = label || "(untitled)";
	f.id = GUI.__native.new_client(f, f.w, f.h);
	return f;
    }
    
    
    /*
     * You can add your own widget types to the bin
     */
    this.new_widget_type = function (name, ctor) {
	__GUI_internal__WidgetBin[name] = ctor;
    }
    
    
    /* 
     * ``Constants'' 
     */
    
    /* label widget alignments */
    this.label = [ ];
    this.label.Left   = 0;	       /* make sure these match guidraw.c */
    this.label.Centre = 1;
    this.label.Right  = 2;
    
    /* client effects */
    this.wm = [ ];
    this.wm.Shade = 1;		       /* keep in sync with guiwm.c */
    this.wm.Iconify = 2;
}
