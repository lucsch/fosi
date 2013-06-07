# BUILD VARIABLES FOR DARWIN (MACBOOK PRO)
MESSAGE ("Darwin user variables...")
SET(CMAKE_PREFIX_PATH /Users/lucien/DATA/PROGRAMATION/_LIB/64/_LIBWXSVN)

# TO USE SYSTEM LIBRARIES FOR GDAL, GEOS, CURL, ETC. COMMENT LINES BELLOW
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} 
  /Users/lucien/DATA/PROGRAMATION/_LIB/64/_LIBCURL 
  /Users/lucien/DATA/PROGRAMATION/_LIB/64/_LIBGIS
) 
SET(CMAKE_FIND_FRAMEWORK "NEVER")

# wxWIDGETS SVN PATH (NOT REQUIRED) 
SET(wxWIDGETS_PATH_SVN "/Users/lucien/DATA/PROGRAMATION/_LIB/64/wxWidgets-svn")
