
      Welcome to the Red Pixel 2 "Way Too Early" Demo (2002-02).


ADMIT IT...
~~~~~~~~~~~

  You already ran the program and got confused, didn't you?  Liar.
  Please at least read the ``STARTING THE PROGRAM'' section.


FIRST THINGS FIRST
~~~~~~~~~~~~~~~~~~

  Red Pixel 2 is a game for multiple players, played over a LAN (and
  maybe in future over high-speed internet (untested)).  It works in
  Linux and Windows, and uses sockets/Winsock (more protocols later).

  RP2 is based on a server with one or more clients connected to it.
  The server manages the game state, the clients draw the pretty
  graphics.  You cannot play without a server and at least one client.


SYSTEM REQUIREMENTS
~~~~~~~~~~~~~~~~~~~

  A 466 MHz Celery manages full speed at the moment.  A P-266 gets
  about 30-40 frames per second (the maximum is 50).  Any machine in
  that range should do ok.

  You need a keyboard and a MOUSE.  Touchpads, trackballs, and those
  rubber finger proddy pointing things between G and H work too, of
  course, but you'll want to kill yourself.

  Your video card needs to support 320x200 in 15-bpp or 16-bpp colour
  depth.  I'll put in screen stretching support (to 640x400, 640x480)
  one day.  Not today.


STARTING THE PROGRAM
~~~~~~~~~~~~~~~~~~~~

  There are four ways to start Red Pixel 2.  There is no nice menu yet,
  so you'll have to use command-line options.


  1. CLIENT-SERVER: You run both the client and the server at the same
     time.  This allows you to play in the game, and also act as a
     server for other players at the same time.

	program -c [-n MY.NICKNAME]

     You will be dropped into a temporary lobby screen.  To start the
     game, press ENTER, then type `,start' (note the comma).  You can
     type any server commands (see below) from this interface, at any
     time.  All server commands typed here need to have the comma,
     otherwise RP2 will think you are sending a text message.


  2. CLIENT ONLY: If you run a client only, you need to connect to a
     running server before you can play a game.

	program -a ADDRESS.OF.SERVER [-n MY.NICKNAME]

     (the address is either a machine name, or an IP address)

     Once you connect you need to wait for the server to start the game.
     You may also connect when the game is already in progress, in which
     case you will be dropped straight into the game.  (Cool, no? :-)


  3. DEDICATED SERVER: You act as a server only, so you will not be able
     to participate in the game yourself.  You won't be able to see the
     game in progress either, because the server will run in a text mode
     (to conserve CPU power, allowing you to run it in the background).
     You may communicate with clients through text messages.

	program -s


  4. ``PARALLEL MODE'': This is sort of like CLIENT-SERVER mode, but
     other players cannot connect to join your game.  This is really
     only for my own testing, but anyway...

	program -p

     It only plays `test.pit' with a single client, then quits.


GAME CONTROLS
~~~~~~~~~~~~~

  In the game:

    - W,A,S,D to move around
    - move the mouse to aim
    - left mouse button fires
    - 1-7 to select weapons
      (Q and E selection keys are not implemented yet)
    - ENTER to type and send a text message to other clients

  In this early demo, pressing Q at any time will quit -- including when
  you are typing a text message.  Sorry.


SERVER COMMANDS
~~~~~~~~~~~~~~~

  These are the server commands.  You can get this list in the dedicated
  server using the `HELP' command.  (It also works in the client-server
  but is less useful there.)

    MAP <filename> - select a map (effective next START command)
    MAP            - display current map
    START          - enter game mode
    STOP           - return to the lobby
    QUIT           - quit completely
    LIST           - list clients
    KICK <id|name> - forcefully disconnect a client
    MSG <message>  - broadcast text message to clients
    CONTEXT        - show current context

  Note: the map you select using the `MAP' command will only take effect
  the next time you enter the `START' command.  So if you are currently
  in a game and wish to try another map, you need to type:

	map anewmap.pit
	stop
	start

  All these server commands are available from the client-server.
  Remember the comma!


RUNNING A DEDICATED SERVER AND DEDICATED CLIENT ON THE SAME MACHINE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  Run the server first:

	program -s

  Then start the game: (typed inside the server screen)

        start

  Then run the client: (with the server still running in another window)

	program [-n MY.NICK]


RUNNING THE MAP EDITOR
~~~~~~~~~~~~~~~~~~~~~~

  Run:

	program -e

  See `editor.txt' in the `doc' directory.


FINALLY...
~~~~~~~~~~

  A lot of stuff is missing or incomplete, and the pickups have not been
  balanced at all.  Sound support was very quickly hacked in.  Still,
  this demo is out because some people have wanted to see it for a long
  time (we're talkin' years), and uni will restart soon (shrug ;-)

  If you have any suggestions or non-obvious bug reports, email
  tjaden@users.sf.net.  No complaints for now.  (Well, if you complain
  there's too much violence, I'll be happy, but there's really not
  enough of it yet.  Don't complaint about the lack of violence.)

  Happy killing! :-)
