/* fileops.js
 *
 * Peter Wang <tjaden@psynet.net>
 */


function Save_The_Map() {	       /* (tm) */
    xml = XMLWrite("RPXmap");
    
    header = xml.child("Header");
    name = header.child("Name");	name.cdata("This is my map");
    author = header.child("Author");    author.cdata("Peter Wang");
    cont = header.child("Contact");	cont.cdata("tjaden@psynet.net");
    
    mapinfo = xml.child("MapInfo");
    mapinfo.attr("width", "128");
    mapinfo.attr("height", "128");
    mapinfo.attr("modes", "Single,Deathmatch");
    
    xml.comment("Base information");
    bases = xml.child("Bases");
    bases.child("Mapping");
    bases.child("");
    
    xml.comment("Entities");
    ent = xml.child("Entities");
    
    xml.comment("Player information");
    ent.child("Players");

    xml.comment("Pickups");
    ent.child("Pickups");
    
    xml.output("zapp.xml");
}
