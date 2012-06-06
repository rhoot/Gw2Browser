Gw2Browser.1 TODO list
======================

* Some restructuring. Decouple views from their data, for instance. Clean up 
headers, etc.

* Support for sub-items, where a file in the .dat can contain child-entries in
the index. This is needed for bank files, EULA, and to a certain degree also
for sound files.

* Support for sound files. (Technically, support for all files!)

* Clean up the model rendering. It's ghastly. I don't particularly like 
rendering pipelines. Sadly, it shows.

* Support for model LODs, collision mesh rendering, and more.

* "Find by fileId" dialog.

* Allow individual file types to add entries to the context menu. For example,
models could get an *export with textures* option.