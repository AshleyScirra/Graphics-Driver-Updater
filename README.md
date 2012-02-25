Graphics Driver Updater
=======================

Your graphics card is the part of your computer that displays applications, games and videos to your screen.  The graphics driver is the program which talks to your graphics card.  Updating your graphics driver can help make your computer run faster and crash less often.

Many computers have out of date graphics card drivers.  The manufacturers release updates, but for various reasons, they don't usually get sent to everyone.  What a shame, given that it's an important part of the overall performance and stability of your computer!

This program is a small, lightweight utility for Windows that helps tell you what your graphics card is and how old its driver is.  It can then send you to the appropriate manufacturer website to update the driver.  It doesn't take long and can fix glitches, crashes and poor performance in many games and applications.

Who made this tool and why?
---------------------------

This tool was created by Scirra (www.scirra.com), developers of the HTML5 game creator Construct 2.  During development, an unreasonable amount of time has gone in to wrestling with buggy, unstable graphics card drivers.  The problem is especially frustrating since unstable drivers can cause our software to crash, which makes us look bad - even though the fault is with the graphics card maker, but the user can't tell, they just see our program crash.  To add to the frustration, often new and fixed versions of the drivers are available, but nobody has installed them.  Bureaucratic manfucaturer agreements or lousy update practices means a significant proportion of general users have out of date drivers, and software developers have to suffer the poor quality drivers that most people end up using, often without the knowledge or information to sufficiently work around every possible problem with every possible graphics card driver.

The problem is even more significant for HTML5 games.  Browsers which support WebGL or hardware-accelerated 2D canvases blacklist old drivers, to stop the user's computer crashing while browsing the web.  This either means the content doesn't run at all, or the browser reverts to slower software rendering.  It's embarrassing that you may have paid good money for a graphics card, but because the manufacturer can't write stable drivers or won't send you working ones, your graphics card remains totally unused.  In many cases updating the driver can fix this, but you're forced to do it manually.

Therefore this tool is designed to make it as easy as possible for end-users to get their graphics card drivers updated.  It's open-source (BSD license) and unbranded.  It's intended for anyone to use at all, in the hopes other developers won't have to suffer the same way due to old drivers.  You may find it useful for:

- recommending it as a step to ensure the user gets the best performance possible before playing a game (especially in HTML5 content).
- sending to users as a diagnostic step to ensure any crashes or glitches being experienced are not due to out-of-date drivers.
- running during your software's install to ensure the user has the latest drivers.
- recommending as a general maintenance/housekeeping for someone's computer.

Making sure everyone has the latest drivers benefits everyone, so please use this wherever you like, uncredited, for any purpose at all.

Technical details
-----------------

The source is a Visual Studio 2008 C++ MFC project.  It should work on Windows XP and newer.