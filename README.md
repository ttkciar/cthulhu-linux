Cthulhu/Linux Game
==================

This is my port of Jason Abbott's "cthulhu" text/curses game to modern Linux.

It is a text/curses-based game where players travel between cities, fight 
monsters, and try to save the world before The Great Old Ones corrupt it 
beyond redemption.

Its documentation is minimal.  See README.quick-help.md for a short command 
reference.  I will be adding more documentation to the wiki as time allows.

The game needs all kinds of improvements, in the build system, gameplay and 
user interface.  The pacing is way off, IMO, and there is insufficient 
support for customizing one's character, though that didn't stop me from 
spending way too many hours playing it as a 1990s b-geek.  I'm inclined to 
incorporate some ideas from "nethack", bsdgames "phantasia", and Pyroto 
Inc's "Pyroto Mountain".

So far I have only built and tested it under Slackware64-15.0, but I expect 
it to work under any Linux with libcurses and libtermcap installed.  Just 
run "make" and you should get an executable called "cthulhu".

Future releases will include an "install" script which will create shared 
directories, set permissions, and copy "cthulhu" to /usr/games, but for now 
I've got "cthulhu" writing all of its files to either $HOME or /var/tmp/ 
which makes it jfw in a single-user non-root environment.

Per Jason's guidance, these sources are published under a CC BY-SA 4.0 license.

For more information on the original game, see notes in https://github.com/ttkciar/cthulhu-ucscb
