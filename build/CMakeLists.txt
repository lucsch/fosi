# CMAKE BUILD FILE FOR FOSI
cmake_minimum_required(VERSION 2.8)

PROJECT ("Fosi")

# SOURCES FILES
FILE (GLOB_RECURSE SRC_FILES ../src/*.cpp)
FILE (GLOB_RECURSE ART_FILES_C ../art/*.cpp)
SET(SRC_FILES ${SRC_FILES} ${ART_FILES_C})
SOURCE_GROUP (source FILES ${SRC_FILES})
FILE (GLOB_RECURSE HEAD_FILES "../src/*.h")
FILE (GLOB_RECURSE ART_FILES_H ../art/*.h)
SET(HEAD_FILES ${HEAD_FILES} ${ART_FILES_H})
SOURCE_GROUP (header FILES ${HEAD_FILES})

#INCLUDE 
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/../src/")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/../art/")

# PLUGINS
SET (GDAL_ROOT "/Users/lucien/DATA/PROGRAMATION/_LIB/64/_LIBGIS")
FIND_PACKAGE(GDAL)
INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})

#SET (CMAKE_FIND_FRAMEWORK "")
FIND_PACKAGE(GEOS PATHS "../plugins/vroomgis/vroomgis/build/cmake/")
INCLUDE_DIRECTORIES(${GEOS_INCLUDE_DIR})

FIND_PACKAGE(PROJ PATHS "../plugins/vroomgis/vroomgis/build/cmake/")
INCLUDE_DIRECTORIES(${PROJ_INCLUDE_DIR})

FIND_PACKAGE(CURL)
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})

#SET (wxWidgets_ROOT_DIR "/Users/lucien/DATA/PROGRAMATION/_LIB/64/_LIBWXSVN")
set(wxWidgets_CONFIG_EXECUTABLE  "/Users/lucien/DATA/PROGRAMATION/_LIB/64/_LIBWXSVN/bin/wx-config")
SET(wxWIDGETS_PATH_SVN "/Users/lucien/DATA/PROGRAMATION/_LIB/64/wxWidgets-svn")
find_package(wxWidgets REQUIRED core base adv qa xml)
include( "${wxWidgets_USE_FILE}" )

# VROOMGIS 
include("../plugins/vroomgis/vroomgis/build/cmake/Use_vroomGISlib.cmake")
include("../plugins/vroomgis/plugins/lscrashreport/build/use_lscrashreport.cmake")
include("../plugins/vroomgis/plugins/wxhgversion/build/use_wxhgversion.cmake")

# Special bundle for APPLE
IF( APPLE )
  #Adding icon and file icon
  SET(APP_ICON_LOCATION "${PROJECT_SOURCE_DIR}/../art/fosi.icns")
  SET_SOURCE_FILES_PROPERTIES(${APP_ICON_LOCATION} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  SET(SRC_FILES ${SRC_FILES} ${APP_ICON_LOCATION} ${APP_ICON_FILE_LOCATION})
ENDIF (APPLE)

SET(BUILD_TYPE "MACOSX_BUNDLE")
IF(WIN32)
  SET(BUILD_TYPE "WIN32")
  SET(SRC_FILES "${SRC_FILES} ../src/resource.rc")
ENDIF(WIN32)

ADD_EXECUTABLE (${CMAKE_PROJECT_NAME} ${BUILD_TYPE}  ${SRC_FILES} ${HEAD_FILES} "../src/resource.rc")
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${GDAL_LIBRARY} ${wxWidgets_LIBRARIES} ${VROOMGIS_LIBRARY} ${LSCRASHREPORT_NAME} ${WXHGVERSION_NAME}) 


# Special bundle for APPLE
IF( APPLE )
  # Set a custom plist file for the app bundle
  set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES 
    MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/custominfo.plist)
ENDIF (APPLE)


