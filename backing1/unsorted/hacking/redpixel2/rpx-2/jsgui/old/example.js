function OkClicked() {
    print(this.parent.ok.label);
    this.parent.kill();
}

function CancelClicked() {
    this.parent.kill();
}

/*
 * GUI.Root.window(type, label, w, h, x, y, name)
 * 
 * GUIWidget.widget(type, ...);
 * 
 * label: type, label, w, h, name
 * textbox: type, value, w, h, name
 * pushbutton: type, label, w, h, name
 */

function main() {
    f = GUI.Root.window("mywindow", "My Window", 200, 100); /* FIXME */
    f.widget("mylabel", "label", "The Label:");
    f.widget("mytextbox", "textbox", "(default text)");
    f.br();
    f.widget("ok", "pushbutton", "Ok");
    f.widget("cancel", "pushbutton", "Cancel");
    f.commit();
    f.children.ok.set_slot("clicked", OkClicked);
    f.children.cancel.set_slot("clicked", CancelClicked);
}

main();
