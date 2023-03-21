# CMAKE BUILD FILE FOR FOSI
cmake_minimum_required(VERSION 3.18) # To support FetchContent / FetchContent_MakeAvailable / SOURCE_SUBDIR

PROJECT ("Fosi")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build ${CMAKE_MODULE_PATH})

# using git version
set(${PROJECT_NAME}_MAJOR_VERSION "1")
set(${PROJECT_NAME}_MINOR_VERSION "0")
include(version)

# defining cmake version for installer mainly
set(CMAKE_PROJECT_VERSION_MAJOR ${${PROJECT_NAME}_MAJOR_VERSION})
set(CMAKE_PROJECT_VERSION_MINOR ${${PROJECT_NAME}_MINOR_VERSION})
set(CMAKE_PROJECT_VERSION_PATCH ${GIT_NUMBER})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# CONAN MAGIC TRICK
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
include_directories(${CONAN_INCLUDE_DIRS})

if (UNIX)
  add_definitions(-std=c++11)
endif (UNIX)

##### VROOMGIS
include(FetchContent)
set(VROOMGIS_GIT_TAG 59e92e6c814ed89b26aa4dc964c8abed25eb1480)
FetchContent_Declare(
        vroomgis
        GIT_REPOSITORY https://github.com/terranum-ch/vroomgis.git
        GIT_TAG ${VROOMGIS_GIT_TAG}
        SOURCE_SUBDIR vroomgis)
message(STATUS "getting vroomgis commit ${VROOMGIS_GIT_TAG}")

FetchContent_MakeAvailable(vroomgis)
include_directories(${vroomgis_SOURCE_DIR}/vroomgis/src)
include_directories(${vroomgis_SOURCE_DIR}/vroomgis/art)

# Export for sourcetrail
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# disable CRT warnings on Windows
if (MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()


# SOURCES FILES
FILE (GLOB_RECURSE SRC_FILES src/*.cpp)
FILE (GLOB_RECURSE ART_FILES_C art/*.cpp)
SET(SRC_FILES ${SRC_FILES} ${ART_FILES_C})
SOURCE_GROUP (source FILES ${SRC_FILES})
FILE (GLOB_RECURSE HEAD_FILES "src/*.h")
FILE (GLOB_RECURSE ART_FILES_H art/*.h)
SET(HEAD_FILES ${HEAD_FILES} ${ART_FILES_H})
SOURCE_GROUP (header FILES ${HEAD_FILES})

#INCLUDE 
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/art/")

# OSX Stuff
set(MACOSX_BUNDLE_ICON_FILE art/fosi.icns)
set(ICON_PATH "${CMAKE_SOURCE_DIR}/art/fosi.icns")
set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

# Executable
ADD_EXECUTABLE (${CMAKE_PROJECT_NAME}
        MACOSX_BUNDLE WIN32
        ${SRC_FILES}
        src/resource.rc
)

add_dependencies(${CMAKE_PROJECT_NAME} vroomgis)
target_link_libraries(${CMAKE_PROJECT_NAME} CONAN_PKG::libpng)
target_link_libraries(${CMAKE_PROJECT_NAME} CONAN_PKG::gdal)
target_link_libraries(${CMAKE_PROJECT_NAME} vroomgis)



# Copy plist to ToolMap (should be after add executable!
configure_file(build/custominfo.plist ${CMAKE_BINARY_DIR}/custominfo.plist)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/custominfo.plist)

# adding multi-core support for Visual Studio Compiler
IF (MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  MESSAGE(STATUS "Adding multiprocessor support for Visual Studio (/MP Flag)")
ENDIF (MSVC)

# Install step for copying *dylib into application bundle on OSX and for changing their reference from
# absolute to relative
IF (APPLE)
  # change cmake_install_prefix from /usr/local to the binary directory
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
  set(APPS ${CMAKE_INSTALL_PREFIX}/bin/${CMAKE_PROJECT_NAME}.app)  # paths to executables
  set(DIRS ${CMAKE_INSTALL_PREFIX}/bin/)  # paths to executables
  INSTALL(CODE "
   include(BundleUtilities)
   fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")
   " COMPONENT Runtime)
ENDIF (APPLE)




