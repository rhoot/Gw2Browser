Gw2Browser
==========

Opens a Guild Wars 2 .dat file and allows the user to browse and extract its 
files. Currently it supports viewing power-of-two textures, models, and binary 
files.

When a .dat file is opened, it will first index the file. This makes it *a lot*
faster to re-open the same .dat. Unfortunately, every time the .dat changes it
will have to be re-indexed (for now).

Keep in mind that if you keep tree items expanded while the indexer is running,
or while an index is being read, the process is *a lot* slower.

The latest binary can always be found [Here](https://drive.google.com/folderview?id=0B0EZlcQoXgS1eXQwUEI3blVNRHc&usp=sharing)

For people who can't run Gw2Browser, download and Install
[Visual C++ Redistributable Packages for Visual Studio 2013](http://www.microsoft.com/en-us/download/details.aspx?id=40784)
choose vcredist_x64.exe for 64bit and vcredist_x86.exe for 32bit executable.

And if it still didn't run, download and install
[DirectX End-User Runtimes (June 2010)](http://www.microsoft.com/en-us/download/details.aspx?id=8109)

Usage
-----

It can be started with:

    Gw2Browser <input dat>

If `<input dat>` is given, the program will open the file as soon as it starts.

Known issues
------------

* **Model rendering is very experimental.** Viewing one may crash the 
browser, or it may not. It might also use the wrong texture, or it might not.
It entirely depends on what model you view, as some models don't have any 
references to textures (mostly equipment models). Some models also use the 
second UV channel for rendering while the shader in the browser only uses the
first. Loading any model is also *far* from optimized!

* **Non-power-of-two dimensions on non-DDS textures are unsupported at the 
moment.** Decompressing one produces garbage blocks, so viewing them is 
disabled until the reason has been determined.

* **Garbage code architecture!** todo : reducing the god objects and moving
privates out of the header files.

Libraries and restrictions
--------------------------

The application is written specifically for MSVC10+, as it links with DirectX 9
and gw2DatTools. It also uses some C++11 features available in said compiler,
since gw2DatTools won't compile without those features anyway.

### Required libraries

* [DirectX SDK June 2010](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* [gw2DatTools](https://github.com/kytulendu/gw2DatTools)
* [wxWidgets 3.0.0](http://wxwidgets.org/)

### Optional libraries

* [Visual Leak Detector](http://vld.codeplex.com/)

If you want to use Visual Leak Detector, remove the "//" at #include <vld.h>
in Gw2Browser.cpp.

Authors
-------

* [Khral Steelforge](https://github.com/kytulendu)

special thanks to:

* [Rhoot](https://github.com/rhoot) : Original Gw2Browser
* [Loumie](https://github.com/ahom) : gw2DatTools
* David Dantowitz : Simple CRC
* [Xtridence](http://forum.xentax.com/viewtopic.php?p=72880#p72880) : modified GWUnpacker to work with Gw2.dat
