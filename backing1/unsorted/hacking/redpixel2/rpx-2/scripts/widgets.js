/* Custom widget types
 *
 * Peter Wang <tjaden@psynet.net>
 */



function scrollbar(min, max) {
    this.value = 0;
    this.min = min || 0;
    this.max = max || 100;
    this.percent = (this.value / this.max) - this.min;
}



function vscrollbar(min, max) {
    this.__parent__ = scrollbar;
    this.__parent__(min, max);
    
    this.resw = 16;
    this.draw = GUI.__native.draw_vscrollbar;
    this.lastmy = -1;
    
    this.changed = function (mb, mx, my) {
	with (this) {
	    if (lastmy == my)
		return;
	    lastmy = my;
	
	    percent = (my-(h/8)) / (h-h/4);
	    if (percent < 0) 
		percent = 0;
	    else if (percent > 1)
		percent = 1;

	    value = parseInt(percent * (max-min) + min);
	    update();
	    signal("value changed", value);
	}
    }
    
    this.button_hold = this.changed;
    this.button_down = this.changed;
}



function hscrollbar(min, max) {
    this.__parent__ = scrollbar;
    this.__parent__(min, max);
    
    this.resh = 16;
    this.draw = GUI.__native.draw_hscrollbar;
    this.lastmx = -1;
    
    this.changed = function (mb, mx, my) {
	with (this) {
	    if (lastmx == mx)
		return;
	    lastmx = mx;
	
	    percent = (mx-(w/8)) / (w-w/4);
	    if (percent < 0) 
		percent = 0;
	    else if (percent > 1)
		percent = 1;

	    value = parseInt(percent * (max-min) + min);
	    update();
	    signal("value changed", value);
	}
    }
    
    this.button_hold = this.changed;
    this.button_down = this.changed;
}




/* 
 * Attach custom widgets
 */

GUI.new_widget_type("vscrollbar", vscrollbar);
GUI.new_widget_type("hscrollbar", hscrollbar);



