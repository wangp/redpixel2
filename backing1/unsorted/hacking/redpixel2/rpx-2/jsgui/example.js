/* example.js		-*- mode: C -*- */

function selector() {		       /* custom widget type */
    this.draw = GUI.__native.draw_filler;
}

function confirm(ques, truelb, falselb, recv, proc) {
    win = GUI.Root.window("confirm", 250, 100, "Confirm");
    label = win.widget("label", "lb", ques, GUI.label.Centre);
    label.resh = 0;	/* I should write a book:
			 * "Advanced JSGUI Techniques" *or*
			 * "JSGUI Secrets Revealed: What they DIDN'T want you to know!"
			 */
    win.br();
    yes = win.widget("pushbutton", "yes", truelb);
    no = win.widget("pushbutton", "no", falselb);
    win.commit();
    
    recv.__confirm_proc = proc;
    win.recv = recv;
    yes.connect("clicked", win, function () { this.recv.__confirm_proc(1); this.kill(); } );
    no.connect( "clicked", win, function () { this.recv.__confirm_proc(0); this.kill(); } );
    
    return win;
}

function __main() {
    GUI.new_widget_type("selector", selector);

    f = GUI.Root.window("tilewin", 200, .5, "Tiles");
    f.widget("selector", "selection");
    f.br();
    f.widget("pushbutton", "ok", "Ok");
    f.widget("pushbutton", "cancel", "Cancel");
    f.commit();

    g = GUI.Root.window("codewin", 300, .6, "Script");
    g.widget("textbox", "text");
    g.br();
    g.widget("pushbutton", "ok", "Commit");
    r = g.widget("pushbutton", "revert", "Revert");
    g.widget("pushbutton", "cancel", "Cancel");
    g.commit();
    
    test = GUI.Root.window("testwin", .3, 100, "Test Window");
    test.br();
    kb = test.widget("pushbutton", "killbut", "Kill me!");
    test.br();
    test.commit();
    
    kb.connect("clicked", test, 
	       function () {
		   confirm("Are you sure to kill me?", "Yes", "No", this,
			   function(value) {
			       if (value) 
				   this.kill(); 
			   } );
	       } );
}

__main();
