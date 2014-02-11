# BUILD VARIABLES FOR LINUX (UBUNTU HOME)

# BUILD VARIABLES FOR DARWIN (MACBOOK PRO)
MESSAGE ("Linux user variables...")

SET (MSYS ON)
SET (USE_MT_LIBRARY OFF)

SET (CMAKE_TOOLCHAIN_FILE /home/lucien/programmation/toolchain/mingw64.tc)
SET (SEARCH_CURL_LIB_PATH /home/lucien/programmation/LIB/_LIBCURL)

# COMMENT THIS LINES TO USE FRAMEWORK 
#SET (SEARCH_GIS_LIB_PATH /home/lucien/programmation/LIB/_LIBGIS)

#MESSAGE (STATUS "Root = ${CMAKE_FIND_ROOT_PATH}")

#SET (WXWIDGETS_PATH /home/lucien/programmation/LIB/_LIBWX)
#SET (CMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE /home/lucien/programmation/LIB/_LIBWX/bin/wx-config)
#SET (wxWIDGETS_PATH_SVN /home/lucien/programmation/LIB/wxWidgets-SVN/)

