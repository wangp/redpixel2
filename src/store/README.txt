
    Store, an access-datafiles-via-strings extension for Allegro.


Introduction

    Store is an abstraction layer for Allegro datafiles that allows
    you to access the items via their names, rather than their index
    numbers.  While this is not new, and can be easily done without
    using an external library, Store uses hash tables to quickly find
    the item you want.

    The idea behind Store is to load many separate datafiles together
    and use them as a single unit.  This one unit is like one huge
    datafile.

    Store does not work with encodings other than ASCII and UTF-8 (for
    speed reasons).  Use only one encoding for all your strings.


Compiling

    Store is small enough to be plugged straight into your project's
    source directory, rather than being linked in as an external
    library (more convenient for end users).  Therefore, no makefile
    is provided -- compile it as if it were your own code.

    The files you need are: hash.c hash.h store.c store.h


API

    Look inside `example.c' for usage.

    int store_init (int);

	Call this at the start of your program to set the size of the
	main hash table.  A larger hash table requires more memory, but
	reduces the chances of a key collision (which will slow down
	retrieval times).  Returns zero on success.

    void store_shutdown ();

	Free all resources, including loaded datafiles.  You must call
	this at the end of your program (will NOT be done automatically).

    int store_load (const char *filename, const char *prefix);

	Load a datafile named FILENAME into core, with a prefix for
	the item names.  Returns a file id on success, or a negative
	number on error.

    int store_load_ex (const char *filename, const char *prefix,
			DATAFILE *(*loader) (const char *filename));

	Same as above, but allows you to provide an alternative
	datafile loading function.

    void store_unload (int id);

	 Unload the datafile using the file id.

    DATAFILE **store;

	All the datafiles you load will be concatenated together into
	a big array, called `store'.

    int store_index (const char *key);

	Returns an index into the `store' data structure for an item
	with key KEY, or zero on error.

    char *store_key (int index);

	Returns the key for a given index, or NULL if not found.
	This function is slow.

    DATAFILE *store_datafile (const char *key);

	Returns a pointer to the datafile structure for an item
	with key KEY, or NULL on error.
	           
    void *store_dat (const char *key);

	Returns a pointer to the datafile structure `dat' field for an
	item with key KEY, or NULL on error.

    DATAFILE *store_file (int id);
    
	Returns a pointer to the DATAFILE structure of file ID, as if
	you had loaded it yourself with `load_datafile', or NULL on
	error.  You will probably not use this function.
	

See also

    Store is based partly off Wumpus, a similar library I wrote last
    year (June 1999).  You might want to take a look at that if Store
    does not do things the way you want.  Or you could just edit the
    source code.


Credits

    Peter Wang mashed this together.

    Jerry Coffin and HenkJan Wolthuis wrote the hash table code,
    which was put in the Snippets collection (public domain).

	
Contact

    Email:	tjaden@users.sourceforge.net
    WWW: 	http://www.alphalink.com.au/~tjaden/
