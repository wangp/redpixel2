function okClicked() {
    print(this.parent.ok.label);
    this.parent.kill();
}



function cancelClicked() {
    this.parent.kill();
}



function main() {
    f = new GUIRoot.window("something", .5, .5, "My Window");
    f.widget("lb", "label", "Name: ");
    f.widget("txt", "textbox");
    f.br();
    f.widget("ok", "pushbutton", "Ok");
    f.widget("cancel", "pushbutton", "Cancel");
    f.commit();
    f.children.ok.setSlot("clicked", okClicked);
    f.children.cancel.setSlot("clicked", cancelClicked);
}


main();


