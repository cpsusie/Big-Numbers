nmake /f makefile.vcwin32 CFG=debug
nmake /f makefile.vcwin32 CFG=release
copy Debug\libpng.lib c:\mytools\lib\MTDebug\libpng.lib
copy Release\libpng.lib c:\mytools\lib\MTRelease\libpng.lib

nmake /f makefile.vcwin32 CFG=debug test
nmake /f makefile.vcwin32 CFG=release test