#!/usr/bin/env python
import os
import cmbui

if __name__ == '__main__':
    # create the cmake cache if needed
    mycmakecache = cmbui.FileCache("cache_cmake.txt")
    mycmakecache.appendQuestion("Use VroomGIS Libs 0 or 1", "SEARCH_VROOMGIS_LIBS")
    mycmakecache.appendQuestion("GIS lib path", "SEARCH_GIS_LIB_PATH")
    mycmakecache.appendQuestion("wxWidgets wx-config (Unix ONLY, keep empty for windows)", "wxWidgets_CONFIG_EXECUTABLE")
    # TODO: This has to be tested under Windows. If it's not working, we could define the 'WXWIN' environement variable !
    mycmakecache.appendQuestion("wxWidgets root directory (Windows ONLY, keep empty for Unix)", "wxWidgets_ROOT_DIR")
    mycmakecache.hasCacheFile(createifneeded=True)

    # create the build cache if needed
    mybuildcache = cmbui.FileCache("cache_build.txt")
    mybuildcache.appendQuestion("Binary directory : ", "BINARY_DIR")
    mybuildcache.appendQuestion("CMake Generators (MSYS Makefiles, Xcode, Visual Studio XX XXXX, etc) : ", "CMAKE_GENERATOR")
    mybuildcache.appendQuestion("build application ? (Y/N)", "BUILD_APPS")
    mybuildcache.hasCacheFile(createifneeded=True)


    # configure Fosi
    print ("Configure FOSI... !")
    myBuild = cmbui.BuildProject(mybuildcache.readCacheValue("CMAKE_GENERATOR"),
                           mycmakecache.readCacheLines(),
                           "",
                           mybuildcache.readCacheValue("BINARY_DIR"))
    print (myBuild.getCmakeCommand(True))

    # create directory if needed
    myBuild.createPath(myBuild.getBinaryDir(), erase_path=False)
    myBuild.doConfigure()

    if (mybuildcache.readCacheValue("BUILD_APPS").upper() == "Y"):
        myBuild.doBuild()











