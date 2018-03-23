libE57Format
==
- v2.0 (not yet released) (initial release)
  - forked from E57RefImpl
  - removed all but the main sources for reading and writing E57 files
  - multiple fixes for compilation on macOS
  - removed BOOST requirement
  - require C++11
  
E57RefImpl
==

- 2013-04-03 roland_schwarz
  - e57unpack now also can handle 2d images
- 2013-01-30 roland_schwarz
  - added missing library identifier string to cmake build
- 2011-10-04 roland_schwarz
  - update to use boost version 1.46.1
  - streamlined cmake files for better static linking
- 2011-03-14 roland_schwarz
  - E57RefImplConfig.cmake added
  - e57validate tool added to cmake
- 2011-03-10: roland_schwarz
  - Added E57RefImplConfig.cmake for user project configuration.
  - The build instructions can be found inside the CMakeLists.txt file.
- 2010-10-16 roland_schwarz
  - e57unpack tool added
  - riegl_1 example files added
