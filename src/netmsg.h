#ifndef __included_netmsg_h
#define __included_netmsg_h


#define NETWORK_PROTOCOL_VERSION	1

/* Libnet has a maximum packet size of 512 bytes, for channels.  Conns
   take up some of those bytes (too lazy to check how many), so we
   impose our own limit. */

#define NETWORK_MAX_PACKET_SIZE		500


/* Network messages are between clients and the server.  Messages are
   distinguished by the first byte in the packet.  The rest of the
   packet contains arguments for that message, if any.  */


enum {
    
    /* The server sends this to a client right after it joins to
       notify it of its registration details.
       
       Args: long client_id.  */

    MSG_SC_JOININFO = 'j',


    /* The client sends this to the server after it joins and receives
       the MSG_SC_JOININFO message.

       Args: char protocol_version, string name.
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


    /* The server sends this to a client to tell it about a client
       that exists (possibly itself).  This is sent once when a new
       client is connected.

       Args: long client_id, string name, string score.
       Score maybe be 0-length.
     */

    MSG_SC_CLIENT_ADD = '+',


    /* The server sends this to a client to tell it that another
       client is no more.  This is sent once when a client is
       disconnected.

       Args: long client_id.
     */

    MSG_SC_CLIENT_REMOVE = '-',


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


    /* The client sends this to the server to request sending a text
       message to all clients.

       Args: string text.
    */

    MSG_CS_TEXT = 't',
    

    /* The server broadcasts this to clients to tell them to display a
       text message.

       Args: string text.
    */

    MSG_SC_TEXT = 'T',


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

    MSG_SC_DISCONNECTED = 'D'
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
	
	CONTROL_LEFT      = 0x01,
	CONTROL_RIGHT     = 0x02,
	CONTROL_UP        = 0x04,
	CONTROL_DOWN      = 0x08,
	CONTROL_FIRE      = 0x10,
	CONTROL_DROP_MINE = 0x20,
	CONTROL_RESPAWN   = 0x40,


    /* Notify server of wish to change weapon.

       Args: string weapon_name.
     */

    MSG_CS_GAMEINFO_WEAPON_SWITCH = 'w'
};


/* Server to Client. */

enum {
    
    /* Tell clients to load a map.

       Args: string filename.
     */

    MSG_SC_GAMEINFO_MAPLOAD = 'l',


    /* Tell clients about a client's new aim angle.  Note: these
       angles are only approximate.

       Args: long client_id, float aim_angle.
     */

    MSG_SC_GAMEINFO_CLIENT_AIM_ANGLE = 'a',


    /* Tell clients about their status.

       Args: long client_id, byte prop, long value.
       where prop is
		'h' for health,
		'a' for ammo,
		'A' for armour
     */

    MSG_SC_GAMEINFO_CLIENT_STATUS = 's',


    /* Tell clients about theirs or other client's scores.

       Args: long client_id, string score.

       The score is a string for displaying only.  As such, it could
       be anything (but usually it would be dealing in death).
     */

    MSG_SC_GAMEINFO_CLIENT_SCORE = '$',


    /* Tell clients to create a new object.

       Args: string type, long object_id, byte hidden_or_not,
	     float x, float y, float xv, float yv, byte collision_tag.

       This is followed by extra fields for the proxy object.
       Each field is of the following format:

       For a float:  'f', string name, float value
       For a string: 's', string name, string value

       The packet is terminated with a null character.
     */

    MSG_SC_GAMEINFO_OBJECT_CREATE = 'c',


    /* Tell clients to destroy an object.

       Args: long object_id.
     */

    MSG_SC_GAMEINFO_OBJECT_DESTROY = 'd',


    /* Tell clients about an object's new position and velocities.

       Args: long object_id, float x, float y, float xv, float yv,
             float xa, float ya.
     */

    MSG_SC_GAMEINFO_OBJECT_UPDATE = 'u',


    /* Tell clients about an object's new hidden status.

       Args: long object_id, byte hidden_or_not.
     */

    MSG_SC_GAMEINFO_OBJECT_HIDDEN = 'h',
    

    /* Tell clients to call an object method.

       Args: long object_id, string method, string arg.
     */

    MSG_SC_GAMEINFO_OBJECT_CALL = 'C',


    /* Tell clients to create some particles.

       Args: char type, float x, float y, long nparticles, float spread.
       where type is 'b' for blood, 's' for sparks, 'r' for respawn particles
     */

    MSG_SC_GAMEINFO_PARTICLES_CREATE = 'p',


    /* Tell clients to create some blod objects.

       Args: float x, float y, long nparticles.
     */

    MSG_SC_GAMEINFO_BLOD_CREATE = 'B',


    /* Tell clients to create an explosion.

       Args: string name, float x, float y, long nparticles.
     */

    MSG_SC_GAMEINFO_EXPLOSION_CREATE = 'e',


    /* Tell clients to create a (visual-only) blast.

       Args: float x, float y, float max_radius, long damage.
     */

    MSG_SC_GAMEINFO_BLAST_CREATE = 'b',


    /* Tell client to play a sound.

       Args: float x, float y, string sound;
     */

    MSG_SC_GAMEINFO_SOUND_PLAY = 'S'
};


#endif
