#ifndef __included_netmsg_h
#define __included_netmsg_h


/* Network messages are between clients and the server.  Messages are
   distinguished by the first byte in the packet.  The rest of the
   packet contains arguments for that message, if any.  */


enum {
    
    /* The server sends this to a client right after it joins to
       notify it of its registration details.
       
       Args: long client_id  (4 bytes).
     */

    MSG_SC_POST_JOIN = 'j',


    /* The server sends this to a client to notify that it is going to
       feed it the game state.  */

    MSG_SC_GAMESTATEFEED_REQ = 'f',

    
    /* A client sends this to the server when it is ready to receive
       the game state.  */

    MSG_CS_GAMESTATEFEED_ACK = 'F',


    /* The server broadcasts this to clients to notify that the game
       is paused.  */

    MSG_SC_PAUSE = 'p',

    
    /* The server broadcasts this to clients to notify that the game
       is resumed.  */

    MSG_SC_RESUME = 'r',


    /* The client sends this to the server to tell the server about
       game information.  This message type has many sub-messages, see
       below.  */

    MSG_CS_GAMEINFO = 'g',


    /* The client sends this to the server when it has sent all the
       game update messages it wants to send for the tick.  */

    MSG_CS_GAMEINFO_DONE = 'G',


    /* The server broadcasts this to clients to tell them about
       changes to the game state.  This message type has many
       sub-messages, see below.  */

    MSG_SC_GAMEINFO = 'h',


    /* The server broadcasts this to clients when it has sent all the
       game update messages it wants for the tick.  */

    MSG_SC_GAMEINFO_DONE = 'H',


    /* The client sends this when it wants to politely disconnect from
       the server.  */

    MSG_CS_DISCONNECT_ASK = 'd',


    /* The server sends this to a client to tell it that it has been
       disconnected (for whatever reason).  After this is sent, the
       server expects no more messages from the client.  */

    MSG_SC_DISCONNECTED = 'D',
};


/* MSG_SC_GAMEINFO and MSG_CS_GAMEINFO messages can contain any number
   of sub-messages (with packet size limitations).  The second byte of
   the packet contains the sub-message type (listed below) and any
   arguments.  The next sub-message then follows behind that, etc.  */


enum {

    /* ---------- Client to Server ---------- */


    /* Notify server of input controls.

       Args: byte input_bitmask  (1 byte).

       The bitmask is as follows:
		0x01	left
		0x02	right
		0x04	up
     */

    MSG_CS_GAMEINFO_CONTROLS = 'c',


    /* Tell server to broadcast text message to all clients.

       Args: long len, char text[]  (4 + len bytes).
     */

    MSG_CS_GAMEINFO_TEXT = 't',


    /* ---------- Server to Client ---------- */

    
    /* Tell clients to load a map.

       Args: long len, string filename  (4 + len bytes).
     */

    MSG_SC_GAMEINFO_MAPLOAD = 'l',
    

    /* Tell clients to create a new object.

       Args: long len, char type[], long object_id, float x, float y
       (16 + len bytes).
     */

    MSG_SC_GAMEINFO_OBJECTCREATE = 'c',


    /* Tell clients to destroy an object.

       Args: long object_id  (4 bytes).
     */

    MSG_SC_GAMEINFO_OBJECTDESTROY = 'd',


    /* Tell clients that an object has been moved.
     
       Args: long object_id, float x, float y  (12 bytes).
     */
    
    MSG_SC_GAMEINFO_OBJECTMOVE = 'm',


    /* Tell clients to display a text message.

       Args: long len, byte text[]  (4 + len bytes).
     */

    MSG_SC_GAMEINFO_TEXT = 't',
};


#endif
