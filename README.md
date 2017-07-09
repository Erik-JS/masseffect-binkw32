# Binkw32 proxy DLL's for Mass Effect 1, 2 and 3

*Original ME3 Autopatcher created by [Warranty Voider](https://github.com/zeroKilo)*

Features:
- All games: Includes ASI Loader;
- ME2/ME3: Enables in-engine console (default keys: TAB, Tilde);
- ME2/ME3: Validates all DLC's;
- ME3: bypasses SSL certificate check (needed for connection with [ME3 Private Server Emulator](https://github.com/PrivateServerEmulator/ME3PSE)).

Tested on: Mass Effect 1 (1.02; Steam), Mass Effect 2 (1.02; Steam and Origin), Mass Effect 3 (Demo and 1.5)

-----------------------------------------------------------

About ASI loader - original code by listener ([XLiveLess](http://gtaforums.com/topic/388658-relgtaiv-xliveless/))<br />
The ASI loader allows for loading of custom-made DLL files without the need of a separate injector.<br />
Such DLL files must be renamed to *.asi and put in a subfolder named *ASI* inside the game's executable folder (*Binaries* for ME1/2, *Win32* for ME3).<br />
For testing purposes, the ASI loader will, alternatively, load plugins from *Binaries*/*Win32* if the *ASI* subfolder is missing or nothing was successfully loaded from there.

**You may visit https://github.com/Erik-JS/ME3-ASI for plugins created by me.**

-----------------------------------------------------------

**Binaries only:** [Dropbox](https://www.dropbox.com/sh/vejorqi4zo54g70/AAAnm0kk7rUq0gPB1U_m5DEwa?dl=0)

-----------------------------------------------------------

**REQUIRED:** [Visual C++ 2015 runtimes](https://www.microsoft.com/en-us/download/details.aspx?id=48145)

**DESIGNED FOR WINDOWS 7 OR LATER.**

-----------------------------------------------------------

Usage:<br />
**Mass Effect 1**: copy binkw23.dll and binkw32.dll to 'Mass Effect\Binaries'.<br />
**Mass Effect 2**: copy binkw23.dll and binkw32.dll to 'Mass Effect 2\Binaries'.<br />
**Mass Effect 3**: copy binkw23.dll and binkw32.dll to 'Mass Effect 3\Binaries\Win32'.

This modification is not made or supported by EA or BioWare.<br />
Mass Effect is a trademark of EA International (Studio and Publishing) Ltd.