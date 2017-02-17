# CMAKE BUILD FILE FOR FOSI
cmake_minimum_required(VERSION 2.8)

PROJECT ("Fosi")

# Version number
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")

# USER DEFINED VARIABLES
#IF (NOT USER_VARIABLES_NAME)
#  SET(USER_VARIABLES_NAME "user-vars-${CMAKE_SYSTEM_NAME}.cmake")
#ENDIF(NOT USER_VARIABLES_NAME)
#IF (USER_VARIABLES_NAME)
#  MESSAGE("Including user defined : ${USER_VARIABLES_NAME}")
#ENDIF (USER_VARIABLES_NAME)
#INCLUDE(${USER_VARIABLES_NAME})

# SOURCES FILES
FILE (GLOB_RECURSE SRC_FILES ../src/*.cpp)
FILE (GLOB_RECURSE SRC_FILES_CORE ../src/core/*.cpp)
FILE (GLOB_RECURSE ART_FILES_C ../art/*.cpp)
SET(SRC_FILES ${SRC_FILES} ${ART_FILES_C} ${SRC_FILES_CORE})
SOURCE_GROUP (source FILES ${SRC_FILES})
FILE (GLOB_RECURSE HEAD_FILES "../src/*.h")
FILE (GLOB_RECURSE HEAD_FILES_CORE ../src/core/*.h)
FILE (GLOB_RECURSE ART_FILES_H ../art/*.h)
SET(HEAD_FILES ${HEAD_FILES} ${ART_FILES_H} ${HEAD_FILES_CORE})
SOURCE_GROUP (header FILES ${HEAD_FILES})

#INCLUDE 
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/../src/")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/../art/")

# VROOMGIS 
SET (SEARCH_VROOMGIS_LIBS ON)
include("../plugins/vroomgis/vroomgis/build/cmake/Use_vroomGISlib.cmake")
include("../plugins/vroomgis/plugins/lscrashreport/build/use_lscrashreport.cmake")
include("../plugins/vroomgis/plugins/wxhgversion/build/use_wxhgversion.cmake")

# Special bundle for APPLE
IF( APPLE )
  #Adding icon and file icon
  SET(APP_ICON_LOCATION "${PROJECT_SOURCE_DIR}/../art/fosi.icns")
  SET_SOURCE_FILES_PROPERTIES(${APP_ICON_LOCATION} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  SET(SRC_FILES ${SRC_FILES} ${APP_ICON_LOCATION} ${APP_ICON_FILE_LOCATION})
  SET(BUILD_TYPE "MACOSX_BUNDLE")
ENDIF (APPLE)

IF(WIN32)
  SET(BUILD_TYPE "WIN32")
  SET(SRC_FILES ${SRC_FILES} "${CMAKE_SOURCE_DIR}/../src/resource.rc")
ENDIF(WIN32)

ADD_EXECUTABLE (${CMAKE_PROJECT_NAME} ${BUILD_TYPE}  ${SRC_FILES} ${HEAD_FILES})
#MESSAGE("Libraries: ${GDAL_LIBRARY}, ${wxWidgets_LIBRARIES}, ${VROOMGIS_LIBRARY}, ${LSCRASHREPORT_NAME}, ${WXHGVERSION_NAME}")
#TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${wxWidgets_LIBRARIES} ${VROOMGIS_LIBRARY} ${LSCRASHREPORT_NAME} ${WXHGVERSION_NAME})
#TARGET_LINK_LIBRARIES(${VROOMGIS_LIBRARY} ${GDAL_LIBRARY} ${wxWidgets_LIBRARIES})

# LINK LIBRARY
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${wxWidgets_LIBRARIES})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${VROOMGIS_LIBRARY})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${WXHGVERSION_NAME})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${LSCRASHREPORT_NAME})

# Special bundle for APPLE
IF( APPLE )
  # Set a custom plist file for the app bundle
  set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES 
    MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/custominfo.plist)
ENDIF (APPLE)


# UNIT TESTING 
IF ( CXXTEST_INCLUDE_DIR )
  ADD_SUBDIRECTORY("../test/build" "${PROJECT_BINARY_DIR}/test")
ENDIF ( CXXTEST_INCLUDE_DIR )


IF (WIN32)
  # INCLUDE ("../plugins/vroomgis/vroomgis/build/cmake/Use_visualstudioMT.cmake")
ENDIF (WIN32)


# INSTALLER

set(INSTALL_DIR_BIN bin)
set(INSTALL_DIR_SHARE share/fosi)
if (WIN32)
    set(INSTALL_DIR_BIN .)
    set(INSTALL_DIR_SHARE .)
endif (WIN32)
 
install ( TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${INSTALL_DIR_BIN})

if (WIN32)
    install (
      PROGRAMS ../bin/Release/gdal111.dll ../bin/Release/geos.dll ../bin/Release/geos_c.dll ../bin/Release/libcurl.dll ../bin/Release/msvcr110.dll
      DESTINATION .
      )
endif (WIN32)
  
# COMMON PROPERTIES

set(CPACK_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fosi - a tool to process plane intersection on a DEM.")
set(CPACK_PACKAGE_VENDOR "Terranum")
set(CPACK_STRIP_FILES ON) # tell cpack to strip all debug symbols from all files

# IDENTIFY ARCHITECTURE

set(CPACK_PACKAGE_ARCH "unkown-architecture")

if(${CMAKE_SYSTEM_NAME} MATCHES Windows)
    if(CMAKE_CL_64)
        set(CPACK_PACKAGE_ARCH "win64")
    elseif(MINGW)
        set(CPACK_PACKAGE_ARCH "mingw32")
    elseif(WIN32)
        set(CPACK_PACKAGE_ARCH "win32")
    endif()
endif(${CMAKE_SYSTEM_NAME} MATCHES Windows)

if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES i686)
        set(CPACK_PACKAGE_ARCH "linux32")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES x86_64)
        if(${CMAKE_CXX_FLAGS} MATCHES " -m32 ")
            set(CPACK_PACKAGE_ARCH "linux32")
        else()
            set(CPACK_PACKAGE_ARCH "linux64")
        endif(${CMAKE_CXX_FLAGS} MATCHES " -m32 ")
    else()
        set(CPACK_PACKAGE_ARCH "linux")
    endif()
endif(${CMAKE_SYSTEM_NAME} MATCHES Linux)

if(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    set(CPACK_PACKAGE_ARCH "mac64")
endif(${CMAKE_SYSTEM_NAME} MATCHES Darwin)

# OS SPECIFIC PROPERTIES

if (APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION}")
    set(CPACK_DMG_VOLUME_NAME "${CMAKE_PROJECT_NAME}")
    set(CPACK_DMG_FORMAT "UDBZ")
endif (APPLE)

if (WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Fosi")
    set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_ARCH}-Setup")
    set(CPACK_PACKAGE_EXECUTABLES "fosi;Fosi")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/cpack/license.txt")
    set(CPACK_NSIS_DISPLAY_NAME "Fosi")
    set(CPACK_NSIS_CONTACT "Terranum info@terranum.ch")
    set(CPACK_NSIS_HELP_LINK "www.terranum.ch")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_NSIS_URL_INFO_ABOUT "www.terranum.ch")
    set(CPACK_CREATE_DESKTOP_LINKS "fosi")
    set(CPACK_NSIS_COMPRESSOR "lzma")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
    set(CPACK_NSIS_MODIFY_PATH "ON")
    # Icon in the add/remove control panel. Must be an .exe file 
    set(CPACK_NSIS_INSTALLED_ICON_NAME Fosi.exe)
    if (CMAKE_CL_64) 
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64") 
    else()
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES") 
    endif()
endif(WIN32)

if (UNIX AND NOT APPLE)
    install(FILES ../art/fosi.ico DESTINATION share/pixmaps)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Terranum <info@terranum.ch>")
    set(CPACK_PACKAGE_VENDOR "Terranum")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS " ...  MRC ... ")
    set(CPACK_PACKAGE_DESCRIPTION "Fosi - a tool to process plane intersection on a DEM.")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
endif(UNIX AND NOT APPLE)
  
include(CPack)