/* mapedit/init.sc -*- mode: C -*- */

export init;

import void create_tiles_table(int size);
import void add_tiles_pack(char *filename);

int init()
{    
    create_tiles_table(200);	       /* 200 entries should be enough */
    
    add_tiles_pack("data/tiles.dat");  /* default tiles pack */
    
    return 0;
}
