Gw2Browser.1
============

Opens a Guild Wars 2 .dat file and allows the user to browse and extract its 
files. Currently it supports viewing power-of-two textures, models, and binary 
files.

When a .dat file is opened, it will first index the file. This makes it *a lot*
faster to re-open the same .dat. Unfortunately, every time the .dat changes it
will have to be re-indexed (for now).

Keep in mind that if you keep tree items expanded while the indexer is running,
or while an index is being read, the process is *a lot* slower.

The latest Win32 binary can always be found at http://skold.cc/gw2browser/

Known issues
------------

* **Model rendering is currently experimental.** Viewing one may crash the 
browser, or it may not. It might also use the wrong texture, or it might not.
It entirely depends on what model you view, as some models don't have any 
references to textures (mostly equipment models). Some models also use the 
second UV channel for rendering while the shader in the browser only uses the
first. Loading them is also unoptimized as hell!

* **Non-power-of-two dimensions on non-DDS textures are unsupported at the 
moment.** Decompressing one produces garbage blocks, so viewing them is 
disabled until the reason has been determined.

* **Garbage code architecture!** Hey, what can I say. I'm a game developer, not
application architecture designer. Plus I'm learning wxWidgets as I go along. I
am working on refactoring chunks of it however, most notably reducing the god
objects and moving privates out of the header files.

Authors
-------

* [Rhoot](https://github.com/rhoot): Main author.

Also contains some code by:

* Xtridence: AtexAsm
* David Dantowitz: Simple CRC

Usage
-----

It can be started as:

    Gw2Browser <input dat>

If `<input dat>` is given, the program will open the file as soon as it starts.

Libraries and restrictions
--------------------------

The application is written specifically for MSVC10+, as it links with DirectX9
and gw2DatTools. It features some C++11 features available in said compiler,
since gw2DatTools won't compile without those C++11 features anyway.

### Required libraries

* [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* [gw2DatTools](https://github.com/ahom/gw2DatTools/)
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