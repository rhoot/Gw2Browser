TODO list
=========

* Port the renderer from D3D9 to OpenGL, run natively on MacOSX or Linux!
(I will try, with my beginner level skill!)

* Some restructuring. Decouple views from their data, for instance. Clean up 
headers, etc.

* Support for sub-items, where a file in the .dat can contain child-entries in
the index. This is needed for bank files, EULA, and to a certain degree also
for sound files.

* Support for sound files. (Technically, support for all files!), see [gw2formats](https://github.com/kytulendu/gw2formats)
need to do 64bit port of gw2formats

* Support for model LODs, collision mesh rendering, and more.

* "Find by fileId" dialog.

* Allow individual file types to add entries to the context menu. For example,
models could get an *export with textures* option.

* External filename database, for known files (such as the exe and dll files).
