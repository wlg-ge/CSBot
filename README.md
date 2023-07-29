# CSBot
CSBot - An IRC bot for games servers

CSBot is an IRC bot for Windows that can publish Counter-Strike, Natural-Selection and Day-of-Defeat games on an IRC channel.
CSBot also has lots of other features. Refer to the website for a full list.


CSBot V0.46 Release notes
by Ilan Tayary (c) 2001, 2002, 2003, 2004
-----------------------------------------

Table of contents:

1. Overview
2. What's new?
3. Command-line options, IRC rcon
4. Known problems / Future plans
5. Contact information
6. Credits
7. Thanks



1. Overview
-----------
CSBot is a server monitor for a Counter-Strike server, but it is also much
much more.
The main features are:
- Easy remote management of a Counter-Strike or Natural-Selection server
- Allows game players to call for a server admin
- Publishes the game on an IRC channel
- Maintains a WonID database of all players for later reference
- Schedules RCON commands to be executed at specified times and intervals
- Allows IRC users to get information about what's going on in the game
- Saves local log files



2. What's new?
--------------
Version 0.46 beta (August 14, 2004)
- Changed default view in "Game Publishing" to "Unknown Game"
- Added ESC and ENTER handling in the connection dialogs (ESC closes dialog, ENTER connects)
- Fixed "round started" message not showing in IRC
- Added log cleaning when logging to disk (removed all id's, made it easier to read for web posting)
- "On connect" commands will now run before /join
- Fixed a restartround bug not updating the player list
- Added game description to the INFO command response

Version 0.45 beta (April 22nd, 2004)
- Fixed a problem that prevented connection to EFNet and several other networks
- Added game server connection timeout setting
- Fixed an issue with game server auto-retry
- Added a validation check on the WonID Database folder setting
- Added <point> tag to the DoD parser
- Various fixes and improvements to the DoD parser

Version 0.44 beta (April 7th, 2004)
- Added logaddress_add support for HLDS x.1.1.1
- CSBot update checks are now performed in the background, so the program itself 
  isn't delayed
- Removed some unneeded grayed out things from the options dialogs
- Now using a NSIS-based installer
- Fixed a problem that caused closing DCC sessions to disconnect from the IRC server
- Added Day of Defeat support.
- Added support for x.1.2.x servers
- Fixed the recycle messages screen (mainly for NS) in the options
- Added support for NS v3.0
- A few graphics fixes in the game publishing options dialog
- Registry settings were moved to UnitedAdmins key
- Added option to require a prefix for commands in the bot's channel (for example: !help)
- Fixed bug that caused the IRC info to stay in the status bar even after disconnection

Version 0.43 beta (August 3rd, 2003)
- QuickFix of a problem in the NS parser that caused wrong messages to be 
  broadcasted in IRC.

Version 0.42 beta (August 1st, 2003)
- Fixed a problem with OutputWnd views in Win9x
- Added LogMod autodetection and support
- Changed game server profile dialogs
- Added support for new Natural-Selection 2.0 features
- Added suicide message in NS parser
- Added recycling messages in NS parser
- Fixed several issues with error messages

Version 0.41 beta (July 12th, 2003)
- Fixed the -game command line option
- Rewrote the whole game server communication part of CSBot. Will now work much
  better behind firewalls and NAT routers
- Fixed a problem with the ACT command in the IRC interface
- Fixed a problem that caused CSBot not to set the channel topic sometimes
- Leading and trailing spaces in hostnames and ip addresses will now be discarded
- Fixed a crash when closing CSBot from the tray icon menu

Version 0.40 beta (June 28th, 2003)
- Fixed a problem with the restart round message
- Fixed a problem that caused players who disconnected to stay in the player list
- Fixed a problem related to channel modes
- Added IRC output queue that queues up to 500 messages, and a delay "clock" that shows
  its state. Will now perform much better in heavy anti-flood situations.
- Added forced-delay system
- Added automatic reconnection to IRC server
- Fixed a problem in the name change message
- Muting the bot will now prevent topic changes as well
- Added "Apply" button to the options dialog

Version 0.39 beta-fix (June 21st, 2003)
- Fixed a problem with player information updates that took place only after outputting
  to IRC :/

Version 0.39 beta (June 21st, 2003)
- Fixed another bug related to 'rcon status'
- Added support for <kills> and <deaths> in IRC messages
- Fixed a bug that caused bogus players to appear in the player list after a while
- Added mine-pack spawn IRC message for NS
- Added support for passworded channels (+k)
- Fixed a bug that caused hidden views to re-appear on the next time you run CSBot
- Added restart-round IRC message for CS
- Added <other> tag for specific variables of IRC messages
- Separated "Killed" and "Killed-Teammate" IRC messages
- Added "Attacked" and "Attacked-Teammate" IRC messages. disabled by default to 
  reduce spammage
- Added an option to show warning about non-logging game server. Enabled
  by default
- Added information about IDENT server responses in the IRC console
- Added an option to close IDENT server automatically after first response. Enabled
  by default
- Reconnection to game server is now enabled by default

Version 0.38 beta (June 15th, 2003)
- Fixed incorrect title on the IRC connection commands dialog
- Added checkbox to enable / disable the ident server for each IRC server profile
- Default IRC interface permissions are HELP and LOGIN (used to be none)
- Several fixes for compiling in VS.NET with MFC 7
- Added automatic reconnection to game server
- Console views now use Guy H's OutputWnd control. This is a lot friendlier to
  the user than a big editbox
- Removed dead say, and dead say_team from NS parser
- Fixed a bug in banning players from the player list
- Added confirmation messages when deleting server profiles
- Color codes will not be sent to +c channels anymore
- Added -guess and -guessharder command line options
- Changed default CS colors for IRC to look better on white background
- Fixed several inter-thread problems
- Fixed wrong help for LOGIN command in IRC interface
- Added the quick setup guide to the package

Version 0.37 beta (June 5th, 2003)
- Bot will not send messages to the channel when not in the channel
- Fixed a WonID Database crash if directory does not exist
- Fixed crash caused by the 'teams' command
- Completely revised the parser engine. Now much more customizable and extensible
  with simple C++ inheritance
- Separated say and say_team messages to dead players and living players.
- Added <ip> tag to IRC messages
- New graphics by Hny
- Added "revert to default colors" button to the color choosing dialog

Version 0.36 beta (May 10th, 2003)
- Now using the XML service of showmyip.com as the primary method for "guessing 
  harder"
- Changed all WonID handling. Now parsed as string instead of number.
- Several preparations towards OpenSource.
- Added OP, DEOP, VOICE, DEVOICE, ACT and SAY commands, only for trusted persons.
- Completely revised the way the interface from IRC and trusted persons work.
- Fixed two bugs in players list context menu.
- Added some more tags that are replaced in the summoning IRC commands
- Fixed problem with /MODE (and potentially other commands)
- Fixed problem in IRC command history
- Last release as "CSBot". From now on let it be called "The G.I.R.L" (Game 
  Information Relay)

Version 0.35 beta (March 1st, 2003)
- Separated IRC colors for CS and NS
- Added even more NS messages. This time mostly about structure destruction.
- Added support for user-defined actions on players in the player list
- When changing options, trusted persons will be logged out only if there 
  were changes in the trusted persons.

Version 0.34 beta (February 22nd, 2003)
- Fixed a UI glitch in Windows XP
- Added more Natural-Selection events for game publishing
- Remade the customizable IRC messages feature
  Your customized IRC messages are once again erased, as they are now
  per-message. Please go over the messages and customize them to your needs
- Added the option to use admin summoning from team chat (not only global game chat)
- Fixed double topic changes when connecting to a game server after connecting to IRC
- Now disconnects correctly from game server and IRC server when Windows is 
  shutting down or logging off
- CSbot now also remembers its window placement (minimized or maximized)
- If the bot's channel is +t (only ops set topic) and the bot gets opped, and the topic
  should be set, the bot will now change the topic
- Added <role2> and <team2> tags so more information can be shown in IRC messages
- Added support for many common IRC meta commands in the IRC console
- Separated the customizable topic message into two. one per each game.

Version 0.33 beta (February 10th, 2003)
- Fixed a bug that caused some message filters not to be saved.
- Improved "Guess Harder" to work with transparent proxies. Will now attempt
  two different methods of guessing. If you get a wrong IP, click on the button
  again to try the alternate method.
- Fixed a bug that caused the on-summon commands not to be saved.
- Game server's on-connect and on-disconnect commands are now per server profile
  Editing them is done in the "Conenct to game server" dialog
- IRC server's on-connect commands are now per server profile. Editing them is 
  done in the "Conenct to IRC server" dialog
- Several UI changes in the WonID DB dialog
- Aded WonID importing capabilities... now you can merge all your databases that
  you gathered all these years :)
- Bot will only change the channel's topic when it is connected to a game server
- Added support for CTCP Version (replies with a notice about version information)
- Added /ctcp command to IRC console

Version 0.32 beta (February 6th, 2003)
- Fixed a bug that caused summoning upon server say
- Fixed a problem with the player info in the IRC interface
- Added player's role to the player info
- Changed "CS Server" to "Game Server" in some places
- "Run Now" in RconSched will now reset the next time for the event
- Added filtering of UDP packets that originate not from the game server

Version 0.31 beta (February 4th, 2003)
- Made new order in the options tabs in preparation for per-game options
- Fixed transparent background that was coming out as white.
- Added antiflood protection
- Fixed /me and /msg handling in the IRC console.
- Text entered in IRC console is now chat text in the bot's channel (and so is /me)
- Started working on the per-game options, some things in the settings are not yet 
  functional even though they seem like they are. don't be too alarmed.
- Fixed a memory leak bug related to the IRC client

Version 0.30 beta (January 23rd, 2003)
- Added basic Natural-Selection support (see http://www.natural-selection.org)
- Added <team2> to IRC messages, used currently for kills only
- Added role change message. uses the <role> tag
- Added "Run Now" command to the context menu of RconSched events

Version 0.29 beta (October 29th, 2002)
- Added adminmod commands support (automatically adds admin_command when needed)
- Many bugs with the trusted persons and the RCON from IRC were fixed
- Help screen in the IRC interface will now show only the available commands
  (this was not working correctly up till now)

Version 0.28 beta (August 12th, 2002)
- Fixed some connection issues
- Added option in CS server profile, whether to redirect the logs
  back to where they were sent before or not.

Version 0.27 beta (July 1st, 2002)
- Added support for more tags in all of the IRC messages. tags that aren't 
  applicable will be deleted from the string
- Channel topic can be now customized in the IRC-General tab in the options.
- Fixed the maximum length of automatic IRC commands (was limited to 50 chars)
- Fixed a bug that changed the color of names that start with a number
- Allows IRC Interface from the csbot's publishing channel
- Fixed bug with bot logins (Password-incorrect)
- Fixed a bug with /msg

Version 0.26 beta (June 26th, 2002)
- Fixed the problem connecting to GamesNet or QuakeNet. CSBot will now work 
  correctly on these two networks

Version 0.25 beta (June 26th, 2002)
- Development is resumed after a long period of it being suspended
  I will get to all those changes that I have on my lists
- Combo boxes instead of editboxes in the IRC console and RCON console, Containing 
  this session's command history
- /QUOTE and /MSG will now work correctly in the IRC console
- New: Automatic CS commands upon connect and disconnect (in the options)
  Previously hardcoded commands were moved here as default commands
  Note that there are still a few commands that are always sent
- 'logaddress' is now set back when disconnecting to what it was before CSBot
  connected
- Gives a warning message if CS server isn't logging ('log' is set to 'off')
- Fixed a problem with binding the socket behind a router/firewall

Version 0.24 beta (February 1st, 2002):
- Added admin summoning feature. Allows 3 types of alerts. WAV file, taskbar flash,
  and IRC commands
- Changed the passwords encryption/decryption routines a little

Version 0.23 beta (January 26th, 2002):
- CSBot Forums are now available at http://forums.csbot.org
- Allows user interface from IRC via Private messages as well as DCC chat
- Login from IRC now requires both username and password (not based on nickname anymore)
- Can now export WonID Database contents to a tab-separated text file
- Added team-color to the TEAMS bot response
- Added some totals to the WonID database dialog
- Passwords that are stored in the registry are now encrypted

Version 0.22 beta (January 18th, 2002):
- Discards wierd characters in player names, so it will not affect IRC output
- Default local ports were changed. (27090 collides with WON authentication port)
- IRC interface to CSBot using private chat. Type "HELP" for a brief command list
- Allows trusted people to login to CSBot via IRC and use priviliged commands
- All previously experienced flicker should now be gone.
- Some minor UI bugfixes

Version 0.21 beta (January 11th, 2002):
- Double-clicking on an event in the RconSched view will now edit it
- Allow /MSG instead of /PRIVMSG in the IRC console
- Customization of IRC colors and messages (Options -> IRC General)

Version 0.20 beta (January 7th, 2002):
- Fixed a slight bug in player-kicking
- Fixed a slight bug in player name-change
- Redesigned the options dialog
- Remember arrangement of child windows between CSBot sessions
- Integrated RconSched into CSBot. It is now able to schedule RCON commands to 
  be sent to the CS server at specified times and intervals.
- CSBot no longer disconnects from IRC when disconnecting from CS
- Allow commands that start with a / (slash) in IRC and auto-exec
- Added controls in the Options dialog for future features (it's just a teaser
  for now)

Version 0.14 beta (January 2002):
- Once more fixed the 3 console views to remember 30k of text (because of 
  Windows limitation)
- Fixed a problem with rcon-responses from server that are more than 1400 
  bytes long
- CSBot sometimes crashed when closing a child window using the "X" button
- Will now be able to receive the raw log even if behind a firewalls (depends 
  on the firewall, though)

Version 0.13 beta (January 2002):
- Added support for command-line options (run with -? for a help message)
- Added search by name in the WonID database
- Log files are now separated to different files according to map	
- Made the console views remember more text (32k instead of 8k)
- Changed the colors in the IRC output to be reversed (white on blue/red)
- Fixed player ban bug that banned by userID
- Fixed IRC channel topic problem on some IRC servers
- Fixed a nasty bug that caused some server profiles to be deleted
- Last connected profile is now selected automatically
- Some toolbar buttons are now pushable to indicate the state of the program

Version 0.12 beta (December 2001):
- First release
- Shows you the raw server log as outputted on the server 
- Allows you to save the raw CS log to local files. 
- Allows you to issue RCON commands via a nice Windows GUI 
- Shows you the server response to the RCON commands 
- Shows you a list of all currently connected players, along with their 
  information 
- Easy user-info/kick/ban directly from the user list (right click on the 
  list to get a menu).
- Can publish what's going on, on an IRC channel 
- Can issue IRC commands from the same window 
- Can execute RCON commands on the CS server by request from an admin on IRC.
- Builds a WonID-to-Nickname database of all players who play on your server 
- Allows you to edit this WonID database 



3. Command-line options
-----------------------
Usage: CSBot.exe [-?] [-game <profilename>] [-irc <profilename>]

-?                  : Shows a short help message about the command-line options
-game <profilename> : Automatically connect to the game server <profilename>
-guess              : Guess local IP before connecting to the game server specified 
                      by -game
-guessharder        : Guess local IP 'harder' before connecting to the game server 
                      specified by -game
-irc <profilename>  : Automatically connect to the IRC server <profilename>

If either -game or -irc are specified, CSBot will be minimized immediately.



4. Known problems /  Future plans
---------------------------------
All known issues are listed in the following forum topic:
	http://forums.unitedadmins.com/index.php?act=ST&f=195&t=29152

For any other problem, please visit our support/feedback forums:
	http://forums.unitedadmins.com/index.php?act=SF&f=194



5. Contact information
----------------------
Please don't hesitate to contact us about anything concerning CSBot.

Stay up to date with the latest about CSBot in the CSBot website:
    http://www.csbot.org

Visit the CSBot forums:
	http://forums.unitedadmins.com/index.php?act=SF&f=194

Register to the CSBot announcements mailing list:
	http://list.unitedadmins.com/mailman/listinfo/csbot

You can reach us via Email at:
    yonatanz@unitedadmins.com - Yonatanz
	eb@unitedadmins.com - Eternal Bliss

or IRC:
    Server: irc.gamesurge.net
    Channel: #csbot



6. Credits
----------
Ilan Tayary (Yonatanz) - Programming
Guy Hachlili (Dawoun) - Programming
puzl - Programming

Eternal_Bliss - Ideas, Testing, Hosting, etc.
Hny - Graphics



7. Thanks
---------
Many thanks must go to all the following people:

GreenBerret, for testing and feedback
swap, for helping me in testing and debugging
Zappatta, for being him :)
All [NoCt] members (http://www.noct.co.il), for keeping me insane
The Israeli Gamers Club - http://www.gamers.org.il
Valve software and the CS and NS Teams, for the great games

All CSBot users whereever they are, for all their help, among them are:
Daniel Roast from Sweden (http://www.onlinecenter.nu)
Kevin Polley from the UK (http://www.thefragzone.co.uk)
FoB|Psyhco from Denmark
sYn|borf and sYn|FatKao from USA
