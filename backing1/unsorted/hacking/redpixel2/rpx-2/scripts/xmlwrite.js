/* xmlwrite.js
 * 
 * Peter Wang <tjaden@psynet.net>
 */


/*
 * Simple XML writer module.  
 * Does not support: namespaces, comments, DTDs, ... and a while lot more.
 * 
 * Still to do:
 * - Output to file rather than print
 * - Convert symbols to &ent;
 */


function _XMLWrite_Attr(name, value) {
    this.name = name;
    this.value = value;
}

function _XMLWrite_Node(name) {
    this.name = name;
    this._att = [];
    this._child = [];
    
    this.attr = function (name, value) {
	a = new _XMLWrite_Attr(name, value);
	this._att.push(a);
    }
    
    this.child = function (name) {
	c = new _XMLWrite_Node(name);
	this._child.push(c);
	return c;
    }
}

function XMLWrite(doctype) {
    
    this.root = new _XMLWrite_Node(doctype || 'nothing');
    
    this._genattrs = function (node) {
	str = '';
	for (i in node._att) 
	    str += ' ' + node._att[i].name + '="' + node._att[i].value + '"';
	return str;
    }
    
    this._output = function (node) {
	if (node._child.length == 0) { /* empty */
	    print('<' + node.name + this._genattrs(node) + '/>\n');
	} 
	else {
	    print('<' + node.name + this._genattrs(node) + '>\n');
	    for (c in node._child) 
		this._output(node._child[c]);
	    print('</' + node.name + '>\n');
	}
    }
    
    this.output = function (filename) {
	if (!filename)
	    return 0;
	
	print('<?xml version="1.0"?>\n');
	this._output(this.root);
	return 1;
    }
}


/* test */
/*
 x = new XMLWrite();
 x.doctype = 'some-kind-o-doc';
 prefab = x.root.child('prefab');

 row = prefab.child('row');
 tile = row.child('tile');
 tile.attr('name', '/tiles/sometile1');;
 row.child('notile');
     
 row = prefab.child('row');
 tile = row.child('tile');
 tile.attr('name', '/tiles/sometile1');;
 tile = row.child('tile');
 tile.attr('name', '/tiles/sometile1');;
     
 x.output('test-file');
 */
