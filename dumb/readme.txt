/*  _______         ____    __         ___    ___
 * \    _  \       \    /  \  /       \   \  /   /     '   '  '
 *  |  | \  \       |  |    ||         |   \/   |       .      .
 *  |  |  |  |      |  |    ||         ||\  /|  |
 *  |  |  |  |      |  |    ||         || \/ |  |       '  '  '
 *  |  |  |  |      |  |    ||         ||    |  |       .      .
 *  |  |_/  /        \  \__//          ||    |  |
 * /_______/edicated  \____/niversal  /__\  /____\usic /|  .  . astardisation
 *                                                    /  \
 *                                                   / .  \
 * readme.txt - General information on DUMB.        / / \  \
 *                                                 | <  /   \_
 *                                                 |  \/ /\   /
 *                                                  \_  /  > /
 *                                                    | \ / /
 *                                                    |  ' /
 *                                                     \__/
 */


********************
*** Introduction ***
********************


Thank you for downloading DUMB! You should have the following documentation:

   readme.txt    - This file
   licence.txt   - Conditions for the use of this software
   release.txt   - Release notes and changes for this and past releases
   docs/
     howto.txt   - Step-by-step instructions on adding DUMB to your project
     faq.txt     - Frequently asked questions and answers to them
     dumb.txt    - DUMB library reference
     deprec.txt  - Information about deprecated parts of the API
     ptr.txt     - Quick introduction to pointers for those who need it
     fnptr.txt   - Explanation of function pointers for those who need it
     modplug.txt - Our official position regarding ModPlug Tracker

This file will help you get DUMB set up. If you have not yet done so, please
read licence.txt and release.txt before proceeding. After you've got DUMB set
up, please refer to the files in the docs/ directory at your convenience. I
recommend you start with howto.txt.


****************
*** Features ***
****************


Here is the statutory feature list:

- Supports playback of IT, XM, S3M and MOD files

- Should be very faithful to the original trackers, especially IT; if it
  plays your module wrong, please tell me so I can fix the bug! (Bear in mind
  that a few features aren't implemented yet though, and please don't
  complain about differences between DUMB and ModPlug Tracker; see
  docs/modplug.txt)

- Accurate support for low-pass resonant filters for IT files

- Very accurate timing and pitching; completely deterministic playback

- Click removal

- Facility to embed music files in other files (e.g. Allegro datafiles)

- Five resampling quality settings, from aliasing to cubic with a linear
  average algorithm to reduce unwanted image frequencies

- Number of samples playing at once can be limited to reduce processor usage,
  but samples will come back in when other louder ones stop

- All notes will be present and correct even if you start a piece of music in
  the middle

- Fast seeking to any point before the music first loops (seeking time
  increases beyond this point)

- Audio generated can be used in any way; DUMB does not necessarily send it
  straight to a sound output system

- Makefiles provided for DJGPP, MinGW, MSVC and Unix (please contact me if
  you'd like to submit or request a makefile for a new platform; the code
  itself should port anywhere that has a 32-bit C compiler)

- Can be used with Allegro, can be used without (if you'd like to help make
  DUMB more approachable to people who aren't using Allegro, please contact
  me)


Since this is the Features section, it seems appropriate to feature a
conversation fragment from IRC EFnet #allegro. If this doesn't inspire you to
set up IRC, I don't know what will! I am 'enthehDUH'.

<enthehDUH> ok, I'm undecided about this
<enthehDUH> I need to choose a default value for dumb_resampling_quality
<enthehDUH> 0 - aliasing
<enthehDUH> 1 - linear interp
<enthehDUH> 2 - linear interp + linear average when downsampling
<enthehDUH> 3 - quadratic interp + linear average when downsampling
<enthehDUH> 4 - cubic interp + linear average when downsampling
<enthehDUH> what do you think? :)
<tero`> best quality
<enthehDUH> that'd be 4
<Inphernic> that quadradigdog thingie sounds good :)
<enthehDUH> but a 486 will have trouble doing 4 :)
<enthehDUH> an early Pentium probably would too
*** Signoff: tero` has left IRC [bye for now]
<enthehDUH> I'm gonna make it 4, I think
<Inphernic> well, make it detect the cpu and make the decision then
<enthehDUH> since the general trend is for computers to get better ;>
<enthehDUH> no, that'd add bloat :O
<Inphernic> bah
<enthehDUH> if I make it 4, ppl will listent to DUMB and say, "hey, this is
   pretty good :)"
<enthehDUH> :)
<enthehDUH> *listen
<Inphernic> what's a little bloat for the good of the mankind :)
<enthehDUH> if I make it 0, ppl will say, "hey, [that other module player]'s
   better :/"
<enthehDUH> :/
<enthehDUH> :P
<Inphernic> um, make the "detection" smaller, like if user_cpu <= PENTIUM
   d_r_q=3; else d_r_q=4;
<enthehDUH> no
<enthehDUH> let the user just set dumb_resampling_quality ;)
<enthehDUH> the examples will have that option built in by the time I've
   finished with them :)
<Inphernic> "Hello and welcome to the DUMB setup! Now you get to choose from
   a variety of options which even the author himself isn't sure of. Now
   please enter the value you wish to use for hypercarbonated atomic audio
   resampling fusion engine's exhaust port's manifold torque :"
<Inphernic> :)
<enthehDUH> :)
<enthehDUH> that's a classic
<enthehDUH> I'm putting it in the docs :)
<Inphernic> :D
<enthehDUH> want ur name in there? :)
<Inphernic> for what? it's not like i've written a single line of code for
   you project? :)
<enthehDUH> no, I mean do you want your name beside the quote :)
<Inphernic> do what you want :)
<enthehDUH> cool :)
<enthehDUH> now this is where ur supposed to tell me to link to ur website ;>
<Inphernic> eek :)
<Inphernic> http://edu.kauhajoki.fi/~juipe/Infini
<enthehDUH> muahahahahhaa
<Inphernic> you're planning something ^Bevil^B and ^Bdevious^B
<enthehDUH> aw crap, how the #%&$ am I supposed to quote bold in the docs :P
<Inphernic> my turn to say muahahahahhaa >D
<enthehDUH> ;>
<Inphernic> oh well, back to.. stuff
*** Inphernic is now known as Inph^Away


*********************
*** What you need ***
*********************


To use DUMB, you need a 32-bit C compiler (GCC and MSVC are fine). If you
have Allegro, DUMB can integrate with its audio streams and datafiles, making
your life easier. If you do not wish to use Allegro, you will have to do some
work to get music playing back. The example program requires Allegro.

   Allegro - http://alleg.sf.net/

In order to use DUMB with Allegro, you must make sure you have made both the
optimised and debugging builds of Allegro; see Allegro's documentation for
details.

If you are using Microsoft Visual C++, you will have to obtain a port of GCC
and GNU Make too. I recommend one of the following:

   DJGPP - http://www.delorie.com/djgpp/
   MinGW - http://www.mingw.org/

The first is a DOS environment, so some Windows users may have trouble with
it. The second should work on all Windows systems. In particular, you need
'make' and 'gcc'. MinGW comes as a single download which contains these
tools, while you have to download them separately for DJGPP. GNU haters, have
no fear; the library or libraries will consist of nothing but MSVC-compiled
code. Furthermore, don't confuse MinGW with Cygwin; MinGW is a native Windows
compiler with a native Windows implementation of the standard libraries,
unlike Cygwin which provides a Unix emulation layer. (I have indeed known
someone refuse to download MinGW because he thought it was Cygwin.)

Read on for details of why GCC and GNU Make are required.

Windows executables have always had their command lines limited to 126
characters, but building DUMB involves command lines longer than this. It is
necessary to use GNU Make, which can pass long command lines to the programs
it uses if they are compatible. A special program called 'runner.c' (which
you can find in the src/misc/ directory) is used to take these long command
lines, write them to a temporary file, and pass the file to MSVC as a script.
GCC is used to compile 'runner.c' so it can take these long command lines
from GNU Make.

If for whatever reason you cannot get DJGPP or MinGW to work, Neil Walker has
kindly uploaded some DUMB binaries at http://retrospec.sgn.net/allegro/ .
They may not always be up to date, so if you opt for this and have trouble,
you're on your own. I strongly recommend making the effort to get DJGPP or
MinGW working.


********************************************
*** How to set DUMB up on DOS or Windows ***
********************************************


You should have got dumb.zip. If for some reason you got dumb.tar.gz instead,
you may have to convert some or all of the text files (certainly to load them
in Notepad, although compilation probably won't be affected). If this
confuses you, just go and download dumb.zip instead.

Make sure you preserved the directory structure when you extracted DUMB from
the archive. Most unzipping programs will do this by default, but pkunzip
requires you to pass -d. If not, please delete DUMB and extract it again
properly.

If you are on Windows, open an MS-DOS Prompt or a Windows Command Line.
Change to the directory into which you unzipped DUMB.

Type ONE of the following lines, depending on which compiler you want to use:

   fix djgpp
   fix mingw
   fix msvc

If you have Allegro, type the following:

   makeall

If you do not have Allegro, type the following:

   makecore

That's right, no spaces! These are batch files that will build and install
both the debugging and optimised libraries for you. Trust me - unless you are
really experienced with DUMB, you need the debugging library.

Please let me know if you have any trouble. Also note that the procedure may
change in future releases.

Once this step is complete, please refer to docs/howto.txt to start using
DUMB! If you use DUMB in a game, let me know - I might decide to place a link
to your game on DUMB's website :)


**********************************
*** How to set DUMB up on Unix ***
**********************************


You should have got dumb.tar.gz. If for some reason you got dumb.zip instead,
you may have to use dtou on some or all of the text files. You may also have
to use chmod on some of them. If this confuses you, just go and download
dumb.tar.gz instead.

To set DUMB up for Unix, first type this in the directory containing DUMB:

   ./fix.sh unix

Then, if you have Allegro, type the following two lines:

   ./makeall.sh
   su -c ./makeall.sh

If you do not have Allegro, type the following two lines:

   ./makecore.sh
   su -c ./makecore.sh

These scripts will build and install both the debugging and optimised
libraries for you. Trust me - unless you are really experienced with DUMB,
you need the debugging library.

The scripts will install into the /usr/local/lib and /usr/local/include
directories by default. The installation will therefore fail on the first
invocation, but the build will be complete. The 'su' command will run the
script as root, so DUMB should be installed properly.

If you cannot be root, or do not want to install DUMB system-wide, you can
use the --prefix option to specify another installation directory, for
example:

   ./makeall.sh --prefix=${HOME}

This will install into your home directory instead, for which you clearly
won't need root privileges. You will only need to run the script once in this
case.

Please let me know if you have any trouble. Also note that the procedure may
change in future releases.

Once this step is complete, please refer to docs/howto.txt to start using
DUMB! If you use DUMB in a game, let me know - I might decide to place a link
to your game on DUMB's website :)


********************************
*** Using the example player ***
********************************


The example player will only have been compiled if you have Allegro. You can
invoke the example by passing a filename at the command line, or by dragging
a music module on top of it in Windows. You can control the playback quality
and troubleshoot problems by editing dumb.ini. Further instructions are
embedded in dumb.ini. That's all there is to know!


*********************************************
*** Downloading music or writing your own ***
*********************************************


If you would like to compose your own music modules, then first I must offer
a word of warning: not everyone is capable of composing music. Do not assume
you will be able to learn the art. By all means have a go; if you can learn
to play tunes on the computer keyboard, you're well on the way to being a
composer!

The best programs for the job are the trackers that pioneered the file
formats:

   Impulse Tracker - IT files - http://www.noisemusic.org/it/
   Fast Tracker II - XM files - http://www.gwinternet.com/music/ft2/
   Scream Tracker 3 - S3M files -
          http://www.united-trackers.org/resources/software/screamtracker.htm

MOD files come from the Amiga; I do not know what PC tracker to recommend for
editing these. If you know of one, let me know! In the meantime, I would
recommend using a more advanced file format. However, don't convert your
existing MODs just for the sake of it.

Note that Fast Tracker II is Shareware. It arguably offers the best
interface, but the IT file format is more powerful and better defined.
Impulse Tracker and Scream Tracker 3 are Freeware. DUMB is likely to be at
its best with IT files.

These editors are DOS programs. Users of DOS-incapable operating systems may
like to try ModPlug Tracker, but should read docs/modplug.txt before using it
for any serious work. If you use a different operating system, or if you know
of any module editors for Windows that are more faithful to the original
trackers' playback, please give me some links so I can put them here!

   ModPlug Tracker - http://www.modplug.com/

BEWARE OF WINAMP! Although it's excellent for MP3s, it is notorious for being
one of the worst module players in existence; very few modules play correctly
with it. There are plug-ins available to improve Winamp's module support, for
example WSP.

   Winamp - http://www.winamp.com/
   WSP - http://www.spytech.cz/index.php?sec=demo

If you would like to download module files composed by other people, check
the following sites:

   http://www.modplug.com/
   http://www.traxinspace.com/

Once again, if you know of more sites where module files are available for
download, please let me know.

If you wish to use someone's music in your game, please respect the
composer's wishes. In general, you should ask the composer. Music that has
been placed in the Public Domain can be used by anyone for anything, but it
wouldn't do any harm to ask anyway if you know who the author is. In most
cases the author will be thrilled, so don't hesitate!

A note about converting modules from one format to another: don't do it,
unless you are a musician and are prepared to go through the file and make
sure everything sounds the way it should! The module formats are all slightly
different, and converting from one format to another will usually do some
damage.

Instead, it is recommended that you allow DUMB to interpret the original file
as it sees fit. DUMB may make mistakes (it does a lot of conversion on
loading), but future versions of DUMB will be able to rectify these mistakes.
On the other hand, if you convert the file, the damage is permanent.


***********************
*** Contact details ***
***********************


If you have trouble with DUMB, or want to contact me for any other reason, my
e-mail address is given below. However, I may be able to help more if you
come on to IRC EFnet #dumb.

IRC stands for Internet Relay Chat, and is a type of chat network. Several
such networks exist, and EFnet is a popular one. In order to connect to an
IRC network, you first need an IRC client. Here are some:

   http://www.xchat.org/
   http://www.visualirc.net/beta.php
   http://www.mirc.com/

Getting on to IRC can be a steep cliff, but it is not insurmountable, and
it's well worth it. Once you have set up the client software, you need to
connect to a server. Here is a list of EFnet servers I have had success with.
Type "/server" (without quotes), then a space, then the name of a server.

   irc.homelien.no
   irc.webgiro.se
   efnet.vuurwerk.nl
   efnet.demon.co.uk
   irc.isdnet.fr
   irc.prison.net

If these servers do not work, visit http://efnet.org/ircdb/servers.php for a
huge list of other EFnet servers to try.

Once you're connected, type the following:

   /join #dumb

A window will appear, and you can ask your question. It should be clear
what's going on from this point onwards. I am 'entheh'. Note that unlike many
other nerds I am not always at my computer, so if I don't answer your
question, don't take it personally! I will usually be able to read your
question when I come back.


******************
*** Conclusion ***
******************


This is the conclusion.


Ben Davis
entheh@users.sf.net
IRC EFnet #dumb
