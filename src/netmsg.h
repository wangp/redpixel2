#ifndef __included_netmsg_h
#define __included_netmsg_h


#define NETWORK_PROTOCOL_VERSION	0


/* Network messages are between clients and the server.  Messages are
   distinguished by the first byte in the packet.  The rest of the
   packet contains arguments for that message, if any.  */


enum {
    
    /* The server sends this to a client right after it joins to
       notify it of its registration details.
       
       Args: word client_id.
     */

    MSG_SC_JOININFO = 'j',


    /* The client sends this to the server after it joins and receives
       the MSG_SC_POST_JOIN message.

       Args: byte protocol_version, word len, byte name[].
     */

    MSG_CS_JOININFO = 'J',


    /* The server sends this to a client to notify that it is going to
       feed it the game state.  */

    MSG_SC_GAMESTATEFEED_REQ = 'f',

    
    /* A client sends this to the server when it is ready to receive
       the game state.  */

    MSG_CS_GAMESTATEFEED_ACK = 'F',


    /* The server sends this to a client after the game state feed is
       done.  After a client receives this, it will enter the 'paused'
       state.  */

    MSG_SC_GAMESTATEFEED_DONE = '*',


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


    /* The server broadcasts this to clients to tell them about
       changes to the game state.  This message type has many
       sub-messages, see below.  */

    MSG_SC_GAMEINFO = 'h',


    /* The server broadcasts this to clients when it is returning to
       the lobby.  */

    MSG_SC_LOBBY = 'l',


    /* The client sends this to the server to initiate the ping
       sequence.  We use it to calculate the amount of lag between
       server and client.  */

    MSG_CS_PING = '?',


    /* The server sends this back to the client in response to a ping.
       It then waits for a boing.  */

    MSG_SC_PONG = '!',

    
    /* The client sends this back to the server in response to a pong.
       The ping sequence is then complete.  */

    MSG_CS_BOING = '@',
    

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


/* Client to Server. */

enum {

    /* Notify server of input controls.

       Args: byte input_bitmask, float aim_angle.
     */

    MSG_CS_GAMEINFO_CONTROLS = 'c',
	
	CONTROL_LEFT  = 0x01,
	CONTROL_RIGHT = 0x02,
	CONTROL_UP    = 0x04,
	CONTROL_FIRE  = 0x08,


    /* Tell server to broadcast text message to all clients.

       Args: word len, byte text[].
     */

    MSG_CS_GAMEINFO_TEXT = 't'
};


/* Server to Client. */

enum {
    
    /* Tell clients to load a map.

       Args: word len, string filename.
     */

    MSG_SC_GAMEINFO_MAPLOAD = 'l',
    

    /* Tell clients to create a new object.

       Args: word len, byte type[], word object_id, float x, float y,
             float xv, float yv, byte collision_tag.
     */

    MSG_SC_GAMEINFO_OBJECT_CREATE = 'c',


    /* Tell clients to destroy an object.

       Args: word object_id.
     */

    MSG_SC_GAMEINFO_OBJECT_DESTROY = 'd',


    /* Tell clients about an object's new position and velocities.

       Args: long object_id, float x, float y, float xv, float yv
     */

    MSG_SC_GAMEINFO_OBJECT_UPDATE = 'u',


    /* Tell clients to display a text message.

       Args: word len, byte text[].
     */

    MSG_SC_GAMEINFO_TEXT = 't',
};


#endif
