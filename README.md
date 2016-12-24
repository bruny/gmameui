# GMAMEUI - It's MAME! On Linux!

## WARNING

This code is migrated from the original SourceForge page. It is old and has issues with more recent versions of MAME. It is unmaintained (due to time and because I no longer run Linux). A more recent fork is available [here](https://github.com/sergiomb2/gmameui).

## About GMAMEUI

GMAMEUI is a front-end program that helps you run MAME on non-Windows platforms, allowing you to run your arcade games quickly and easily.

GMAMEUI is a fork of the defunct <a href="http://gxmame.sourceforge.net">GXMame project</a>.

It contains a number of enhancements over GXMame:
* Support for <a href="http://rbelmont.mameworld.info/?page_id=163">SDLMame</a>
* Support for more recent versions of MAME (currently supporting up to v0.123)
* Support for the recent features introduced to MAME (the last version supported by GXMame was 0.95)
* Migration to Glade for UI, allowing easier maintenance
* A substantial number of bug fixes and UI improvements over GXMame

## Screenshots

<a href="images/main_window.png"><img src="images/t_main_window.png"></a>
<a href="images/default_prefs.png"><img src="images/t_default_prefs.png"></a>
<a href="images/rom_properties.png"><img src="images/t_rom_properties.png"></a>
<a href="images/dir_selection.png"><img src="images/t_dir_selection.png"></a>

## Download

The latest version of GMAMEUI is 0.2.12, released 25 October 2009. GMAMEUI can be [downloaded directly from Sourceforge](http://sourceforge.net/project/platformdownload.php?group_id=217857).

0.2.12 (released 25 October 2009) adds the following features:

* Add support in SDLMAME for finding effect files. Any .png file in the specified MAME artwork directory will be selectable in the MAME Options dialog (SF Feature Request 2780394)
* Enhancements to ROM Info dialog:
** Add additional details to ROM info screen (including ROMs sharing the same driver and clones of the ROM)
** Add button to open the ROM's zip file
* Start migration of screens to GtkBuilder from deprecated libglade
* Fix Debian policy violation bug (Debian Bug 547699)
* Bug fixes:
** Compilation errors (SF Bug 2848961)
** Support for -keymap and -keymap_file options (SF Bug 2847216)
** Locales using commas now work correctly with the gamelist version (SF Bug 2847787)

0.2.11 (released 29 August 2009) adds the following features:

* Automatically search for sdlmame in the user's $PATH and add it
* When adding a MAME directory, GMAMEUI will now automatically look for related directories and add them
* Show progress bar when loading the gamelist at startup
* Rebuilding the gamelist now automatically triggers an audit afterwards
* Move all GMAMEUI config files to ~/.config/gmameui and ~/.config/mame to comply with XDG standards
* Improve speed of switching between filters on the LHS and the filter buttons at the top of the gamelist
* Remove items from the gamelist file that are not required on startup and only load them when opening the ROM Properties dialog (this requires a gamelist rebuild)
* Improve speed of switching between filters on the LHS and the filter buttons at the top of the gamelist
* Prompt user to rebuild gamelist if it is out of date or missing at startup
* Update some XMAME default values
* Add processing of individual ROMs and SHA1 values
* Redundant source code files have been removed
* Completely updated translations for French (fr), German (de), Polish (pl) and Traditional Chinese (zh_TW) - thanks to all who contributed!



## What Do I Need To Run GMAMEUI?
GMAMEUI requires a Linux version of MAME (SDLMAME, XMAME) to have been installed. When you run GMAMEUI, you will be asked to locate the MAME version. SDLMAME is available <a href="http://rbelmont.mameworld.info/?page_id=163">here</a>, and you can download XMAME (even though it now appears to be dead) <a href="http://x.mame.net/">here</a>.
MAME is, of course, rather useless without ROMs. You can also specify the directory containing your ROM files - and can specify multiple directories if you have your ROMs in different directories.
You can also point GMAMEUI to the directories for your samples, artwork, flyers and snapshots, amongst others. Click on <a href="links.html">Links</a> for a list of websites where you can download these MAME resources.

## How Can I Contribute to GMAMEUI?
You can help the development by reporting bugs, contributing patches or translations.

## Why Fork GXMame?
GXMame was written by shadow_walker and was designed to be a front-end for XMAME. XMAME has not been actively maintained since 2006 and v0.104, which coincided with some major changes in MAME's graphics code.
SDLMAME, on the other hand, has kept up to date with the MAME changes, and is now the preferred MAME emulator for Linux, but was not supported by GXMame. GXMame now appears to be dead, given that the latest release was 0.34b in December 2003.
GMAMEUI has extended GXMame by adding support for SDLMAME, fixing some of GXMame's open bugs, and adding useful enhancements.

## Old Releases

0.2.10 (released 3 July 2009) adds the following features:

* Replace non-GPL zip handling code
* Add new GMAMEUI Preferences option to prefer either custom icons (using icons.zip) or status icons (SF Bug 2011168)
* Fix some broken legacy XMAME options handling
* New French translation, updated Italian and Portugese translations
* Fix some compilation/runtime GTK warnings

0.2.9 (released 5 June 2009) adds the following features:


* Add search field for the gamelist, performing a case-insensitive search on the ROM name
* Add buttons to filter on all, available and unavailable ROMs
* Set row height to be consistent whether custom icon displayed or not
* Handle changes to display in the GUI Preferences screen immediately (i.e. column changes)
* Use italic, rather than text colour, to distinguish Clones
* Removed the option to display the gamelist with clones nested as a tree; the Gamelist now only displays in a GtkListView
* Added new filter Imperfect Driver
* Add support for -waitvsync option (SF Bug 2800305)
* Migrate gamelist to a GObject
* Migrate RomEntry struct to a GObject

WARNING - since there are a number of significant changes 'under the hood' this release should be considered beta quality - please report any bugs you find.

0.2.8 (released 21 April 2009) adds the following features:

* Directories window warns if no ROM or MAME executable chosen
* Add new filter group Status, filters Correct and Best Available, and add ROM availability filter buttons (SF Feature Request 2474989)
* Fix crash when removing last MAME executable directory in Directories window, prompt user if no executable or ROM path is set when closing the Directories window
* Fix segfault on startup if there is no .gmameui directory (SF Bug 2738775)
* If no MAME executables are defined when starting, user is prompted to open the Directories window
* When switching to a different MAME executable, prompt to rebuild the gamelist
* Added Swedish (sv) translation (thanks Daniel Nylander) (SF Feature Request 2544027)
* Code cleanups

0.2.7 (released 29 March 2009) adds the following features:

* Disable options not supported by the current MAME version in options dialog
* Add support for additional command-line options up to 0.127
* Use Glade-generated dialog for XMAME, consolidate code for generating command-line
* Various code fixes and cleanups

0.2.6 (released 4 October 2008) adds the following features:

* Fix crash when rebuilding the gamelist (Gentoo bug 213622, SF bug 2127471)
* Resize displayed image as available space increases/decreases
* Audit window does not revert to smaller size if expander is clicked
* Replace use of memset in filters_list
* Fixed some GError-related errors
* Fix compilation errors

0.2.5 (released 20 September 2008) adds the following features:

* GMAMEUI now supports MAME 0.127. Fixed SF bug 2076562  (unknown option: -yuvmode) by validating supported MAME options prior to launching ROM
* Modified handling of MAME audit processing to catch ROM errors and properly display invalid ROMs. Samples now shows Not Available samplesets
* Italian translation 100% (thanks Ugo Viti)
* Missing/invalid ROMs now reported properly upon launch
* Main window remembers settings on disabling/enabling filter list, sidebar, toolbar and statusbar on startup
* If no preferences file is available, or values are missing, use default values for the UI
* Load and set joystick value in GUI Prefs dialog
* Mark new source files as translateable, mark new strings as translateable, modify Makefile for better RPM support (SF patch 2035285 - thanks Ugo Viti). Italian translation is now at 100%.
* Set default button on the dialogs to be the Close button
* Added support to launch the GMAMEUI Help manual. This will use gtk_show_uri (if GTK > 2.13 is installed), or revert to libgnome's gnome_url_show. Users who do not wish to compile in libgnome (e.g. XFCE or KDE users) will need to explicitly disable it by compiling with --disable-libgnome, but will not be able to access the manual from within GTK
* Apply patch to allow RPM build (thanks Ugo Viti)
* Other code changes
** Consolidated code to retrieve images from zip files
** Fix many compilation warnings

0.2.4 (released 27 July 2008) adds the following features:

* Fixed segfault when closing the Directories window with no ROM or Sample paths set
* Minor UI changes
** Emulation can be started by pressing Enter
** Set minimum column width based on column's title
** Add samples as a toggle column in Gui Prefs dialog
** Update gamelist view after Gui Prefs dialog is closed, rather than on restart
** Ellipsize text columns in filters list and gamelist when size is reduced
** Remove filter horizontal scrollbar
** Changing the MAME executable is retained on restart (fixing regression bug)
** Fix keyboard shortcuts
** Add support for setting cpanel directory
** Scroll to the last selected ROM on startup (fixing regression bug)
** Samples filter now works again (fixing regression bug)

* Playing back input (.inp) files no longer requires user to have selected the matching ROM. GMAMEUI automatically opens the ROM based on the .inp filename

* Recording input files silently records to the .inp directory, creating it if it doesn't exist

* Code refactoring

** Migrating gamelist to GObject for easier refactoring and maintenance
** Migrate main window to Glade for UI generation
** Fix some startup errors/warnings, particularly GTK-Critical ones relating to g_key_file and GError
** Directories window now instant-apply, removing redundant save code on exit

* Added configure switch --disable-doc to avoid building documentation on non-Gnome systems



0.2.3 (released 22 June 2008) adds the following features:


* New GMAMEUI UI Preferences dialog, and preferences handling. Most preferences are now saved automatically as they changed (instant apply). The gmameui.ini file format has been changed, and the dirs.ini file will be made obsolete.
* Add support for OpenGL options, and corresponding Options dialog page (SourceForge feature request 1977184)
* Add support for -effect option (e.g. scanlines). Note that this is hard-coded to a set of pre-defined effects, and doesn't distinguish between available/unavailable effects. The effects .png files need to be placed in the artwork directory
* Various bug fixes and code cleanups
* Setup directory and build structure for Help
* New translations:
** Italian translation (Ugo Viti)
** Brazilian translation (Junix)

0.2.2 (released 15 May 2008) adds the following features:

* Fixed bug which caused ROMs not to be filtered in Availability filter categories (bug 1962183)
* Remove a number of non-visible columns from the TreeView store, resulting in a speed up when populating the ROM list
* Code cleanup, remove redundant code, fixed some compile/run-time GTK warnings
* Removed Drivers as a filters category for 2 second improvement when starting
* Replaced g_list_insert_sorted with g_list_append when adding a ROM to the gamelist, which was unnecessary and cost 2 seconds on startup
* Fixed setting ROM- and executable-specific menu and toolbar items sensitive when no ROM and/or executable is selected

0.2.1 (released 10 May 2008) adds the following features:

* Change the filter sidebar layout to be similar to that of Rhythmbox, rather than the previous ugly treeview with folder icons
* Add new classes GMAMEUIFiltersList and GMAMEUIFilters for filtering game list
* Use integer enums in RomEntry rather than strings
* Reduce some of the memory leaks in RomEntry
* Fix hang on subsequent OK in Directory Selection window caused by invalid free's
* Fix bug when saving directories with different number of ROM paths and sample paths caused by bad copy-and-paste

0.2 adds the following features:

* Add new Options preference dialog for SDLMAME, which handles the newer options available. XMAME is supported by the older-style options window
* New ROM Properties window, separated from the Options window
* New column layout window
* Use GtkAction to support popup and toolbar/menubar creation
* Code cleanups and re-organisation, particularly around Sidebar
