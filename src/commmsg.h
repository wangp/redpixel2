#ifndef __included_commmsg_h
#define __included_commmsg_h


/* client to server */
enum {
    
    /*
     *  MSGs are game messages, and passed through to the game layer.
     */

    MSG_CS_KEY_LEFT = 'l',	/* id[4] */
    MSG_CS_KEY_RIGHT = 'r',	/* id[4] */
    MSG_CS_KEY_UP = 'u',	/* id[4] */

    /*
     * 	XMSGs are processed by the comm layer and not the game layer. 
     */

    /* The client sends this xmessage when it has sent all the input it 
     * wants to send for the tick.  */
    XMSG_CS_OVER = 'x'
};


/* server to client */
enum {
    
    /*
     *  MSGs are game messages, and passed through to the game layer.
     */
    
    MSG_SC_MOVE_OBJECT = 'm',	/* id[4]; x[4]; y[4] */

    /*
     * 	XMSGs are processed by the comm layer and not the game layer.
     */

    /* The server sends this xmessage to the client when it has sent all the 
     * messages it wants for the tick, and the client should continue with
     * the next tick.  */
    XMSG_SC_TICK = 't'
};


long msg_get_long (const unsigned char *buf);
void msg_put_long (unsigned char *buf, long l);
float msg_get_float (const unsigned char *buf);
void msg_put_float (unsigned char *buf, float f);


#endif
