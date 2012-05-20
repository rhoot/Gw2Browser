Gw2Browser.1
============

Opens a Guild Wars 2 .dat file and allows the user to browse and extract its 
files. Currently supports viewing power-of-two textures (aside from DXTA) and
binary files. The decompression algorithm for non-power-of-two textures is yet
to be figured out.

When a .dat file is opened, it will first index the file. This makes it *a lot*
faster to re-open the same .dat. Unfortunately, every time the .dat changes it
will have to be re-indexed (for now).

Keep in mind that if you keep tree items expanded while the indexer is running,
or while an index is being read, the process is *a lot* slower.

The latest Win32 binary can always be found at http://skold.cc/gw2browser/

Authors
-------

* [Rhoot](https://github.com/rhoot): Main author.

Also contains some code by:

* Xtridence: AtexAsm
* David Dantowitz: Simple CRC

Usage
-----

It can be started as:

    Gw2Browser -i <input dat>

If `<input dat>` is given, the program will open the file as soon as it starts.

Libraries and restrictions
--------------------------

The application will not compile with any compiler aside from MSVC unless said
compiler gets some non-standard defines added to 'stdafx.h'. The application 
*may* compile and run in Linux or MacOS X if that is done however. RTTI needs
to be enabled in order to compile (if you can dynamic_cast, you're covered).

### Required libraries

* [gw2DatTools](https://bitbucket.org/Daegalus/gw2re/)
* [wxWidgets](http://wxwidgets.org/)

Keep in mind gw2DatTools is still under development and some modifications may
be necessary to compile.

### Optional libraries

* [Visual Leak Detector](http://vld.codeplex.com/)

If you do not want to use Visual Leak Detector, remove the include for vld.h
in Gw2Browser.cpp.

Contributing
------------

For contributions, please go to [this project's page on GitHub](https://github.com/rhoot/Gw2Browser).