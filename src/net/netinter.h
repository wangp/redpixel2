#ifndef __included_netinter_h
#define __included_netinter_h


#define PACKET_NONE		0
#define PACKET_REGISTER		1
#define PACKET_MESSAGE		2
#define PACKET_DISCONNECT	3
#define PACKET_COMMAND		4
#define PACKET_BADCOMMAND	5
#define PACKET_PING		6
#define PACKET_PONG		7


#define STATE_CONNECTING	0
#define STATE_FAILURE		1
#define STATE_REGISTER		2
#define STATE_CONNECTED		3
#define STATE_DISCONNECT	4
#define STATE_DISCONNECTING	5
#define STATE_DISCONNECTED	6


#define TIME_RATE		50

#define TIME_SERVER_DISCONNECT (TIME_RATE * 5)
#define TIME_SERVER_REGISTER   (TIME_RATE * 30)
#define TIME_SERVER_SHUTDOWN   (TIME_RATE * 10)

#define TIME_CLIENT_DISCONNECT (TIME_RATE * 5)
#define TIME_CONNECT_PERIOD    (TIME_RATE * 1)

extern volatile int net_time;


extern NET_DRIVERNAME *net_drivers;


#endif
