The X-Plane Plugin Website is:

http://www.xsquawkbox.net/xpsdk/

X-Plane SDK Release 1.0.2 1/5/05

The headers of the SDK are modified to support Kylix.  No changes for Mac, Windows, or C users.  Headers now have SDK version numbers.

X-Plane SDK Release 1.0.1 12/29/04

The headers of this SDK are modified to support Linux complication.  No changes for Mac and Windows users.

X-Plane SDK Release Candidate 1

Only one slight change in the enums: the enum xpProperty_SubWindowHasCloseBoxes in XPStandardWidgets.h has been changed to xpProperty_MainWindowHasCloseBoxes.  Its value has not been changed, so you will need to search-and-replace your code when using this version of the SDK, but already-compiled plugins will experience no different operation.

The documentation has been revised for all headers to revise changes made to the SDK over the course of beta.

X-Plane SDK Beta 5

This version of the SDK features a number of enumeration changes to reflect the X-Plane interface more correctly.  This became crucial when X-Plane 7's new user interface was released.  With X-Plane in release candidates hopefully beta 5 of the SDK could be the last one.  Please see:

www.xsquawkbox.net/xpsdk/newui.html

For a comprehensive description of all the enumeration changes.  For most plugins (no developers reported using the deprecated enumerations), a simple search and replace should suffice.  Plugins compiled against the beta 4 SDK that do not use now-unsupported graphics will continue to work correctly.

X-Plane SDK Beta 4

This release corrects two problems with the Pascal headers: function pointer types are now declared cdecl (since this is how the SDK calls them), and the import library for the widget callbacks is now XPWIDGETS.DLL as it should be.

X-Plane SDK Beta 3

What's New:

This release finally features full documentation and a stable widgets API, as well as a few other minor bug fixes.

Starting with beta 3, the DLLs necessary to run plugins ship with X-Plane 660.  The SDK will work with X-Plane 660 RC3 and later.  The XPWidgets DLL now lives in the Resources/plugins folder.

Starting with beta 3, extra plugins, documentation, sample code, and sample projects are now featured directly on the web in the new X-Plane SDK library.  They are not included in the SDK zip file; the zip file only contains headers and lib files for the SDK.

X-Plane SDK Beta 2

WHAT'S NEW

You must recompile your plugin for the beta 2 plugin SDK!  Plugins compiled against the beta 1 SDK will not work with X-Plane 660 or the new XPLM.DLL.

A huge number of data refs have been added.  Unfortunately the documentation is thin.  Use the data ref tester plugin to view the data refs in real time and find what you need.

The data ref APIs have also changed to allow for arrays of integers as well as floats.  Some sim variables are now arrays that were previously many individual items.

A new drawing phase is available for replacing aircraft graphics.  The texturing APIs in XPLMGraphics have been revised.  The most notable change is that you cannot use the SDK to load your textures.  (This functionality was broken and never worked in beta 1.)  See the x-plane-dev list for sample code on how to load your own bitmaps.

X-Plane can reload plugins on the fly.  Use the Plugin Enabler plugin to reload your plugin.  On the Mac you can throw the old DLL in the trash and put a new one in its place to reload a new version of the plugin.  On the PC, an alert comes up; while this alert is up you can swap your plugins' DLL.  This allows you to recompile your plugin without rebooting the sim.

Delphi Pascal interfaces and sample code are in the SDK.  Thanks to Billy Verreynne for his hard work on this.

X-PLANE SDK Beta 1

CONTACTS AND LINKS

The X-Plane SDK home page is located at:

http://www.xsquawkbox.net/xpsdk/

On this site you will find the latest SDK, an online version of all the documents, as well as other information.

The X-Plane SDK authors can be reached at:

xplanesdk@xsquawkbox.net

Please do not email Austin for SDK questions or support; the SDK is a third party effort.

the X-Plane developer mailing list is an unlisted yahoo group frequented by many X-Plane developers.

x-plane-sdk@yahoogroups.com

GETTING STARTED

This SDK download contains headers for Windows, Macintosh and Linux for both C and Delphi plugins, as well as import libraries to link against for Macintosh (CFM) and Windows.  (Import libraries are not needed in Linux and Macintosh Mach-O plugins.)

Documentation, sample plugins, sample code, project files and make files for all platforms are provided via the X-Plane SDK web site.  Sample project files are provided for Microsoft Developer Studio 6 on Windows and Metrowerks CodeWarrior Pro 8 on Macintosh.  Make files are provided for Linux.  There are Delphi project files provided which will work with Kylix on Linux.

For more info please see the SDK web site: http://www.xsquawkbox.net/xpsdk/
