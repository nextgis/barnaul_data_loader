################################################################################ 
#  Project:  nextgis manager plugin, Barnaul data loader 
#  Purpose:  cmake script
#  Author:   Dmitry Baryshnikov, dmitry.baryshnikov@nextgis.com
################################################################################
#    Copyright (C) 2015 NextGIS, info@nextgis.com
# 
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
################################################################################
cmake_minimum_required (VERSION 2.8.10 FATAL_ERROR)

set(PROJECT_NAME ngm_barnaul_dataloader)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
set(CMAKE_COLOR_MAKEFILE ON)

# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# parse the version number from version.h and include in wxGISMON_MAJOR_VERSION and wxGISMON_MINOR_VERSION
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h VERSION_H_CONTENTS)
string(REGEX MATCH "MAJOR_VERSION[ \t]+([0-9]+)"
  MAJOR_VERSION ${VERSION_H_CONTENTS})
string (REGEX MATCH "([0-9]+)"
  MAJOR_VERSION ${MAJOR_VERSION})
string(REGEX MATCH "MINOR_VERSION[ \t]+([0-9]+)"
  MINOR_VERSION ${VERSION_H_CONTENTS})
string (REGEX MATCH "([0-9]+)"
  MINOR_VERSION ${MINOR_VERSION})
string(REGEX MATCH "RELEASE_NUMBER[ \t]+([0-9]+)"
  RELEASE_NUMBER ${VERSION_H_CONTENTS})  
string (REGEX MATCH "([0-9]+)"
  RELEASE_NUMBER ${RELEASE_NUMBER})

# Setup package meta-data
set(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE_NUMBER})
message(STATUS "version=[${VERSION}]")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include(lib)


#find needed packages
find_package(GDAL REQUIRED)
if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})

    FOREACH(file_path ${GDAL_INCLUDE_DIR})       
        include_directories(${file_path}/ogr)
        include_directories(${file_path}/ogr/ogrsf_frmts)
        include_directories(${file_path}/port)
        include_directories(${file_path}/gcore)
        include_directories(${file_path}/alg)
        include_directories(${file_path}/frmts/vrt)
    ENDFOREACH()   
endif(GDAL_FOUND)

if(UNIX)
    option(HAVE_GTK_INTEGRATION "Set ON to build with GTK integration" OFF)
    if(HAVE_GTK_INTEGRATION)
        option(HAVE_UNITY_INTEGRATION "Set ON to build with Unity integration" OFF)
    endif()
endif()

if(HAVE_GTK_INTEGRATION)
     find_package(GTK3 QUIET)
     if(GTK3_FOUND)
        include_directories(${GTK3_INCLUDE_DIRS})
        add_definitions(-DHAVE_GTK3)
        message(STATUS "GTK3 libs=[${GTK3_LIBRARIES}] headers=[${GTK3_INCLUDE_DIRS}]")
     else()
        find_package(GTK2 REQUIRED)
        if(GTK2_FOUND)
            include_directories(${GTK2_INCLUDE_DIRS})
            add_definitions(-DHAVE_GTK2)
            message(STATUS "GTK2 libs=[${GTK2_LIBRARIES}] headers=[${GTK2_INCLUDE_DIRS}]")
        endif()
    endif()
endif()

if(HAVE_UNITY_INTEGRATION)
     find_package(UNITY REQUIRED)
     if(UNITY_FOUND)
        include_directories(${UNITY_INCLUDE_DIR})
        add_definitions(-DHAVE_UNITY)
    endif()
    message(STATUS "UNITY libs=[${UNITY_LIBRARIES}] headers=[${UNITY_INCLUDE_DIR}]")
endif()

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
set(wxWidgets_USE_REL_AND_DBG ON)
find_package(wxWidgets 2.9 REQUIRED core base xml net adv aui propgrid)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif()

find_package(WXGIS COMPONENTS core framework catalog catalogui geoprocessing geoprocessingui REQUIRED)
if(WXGIS_FOUND)
    include_directories(${WXGIS_INCLUDE_DIR})
endif()

find_package(POSTGRES REQUIRED)
if(POSTGRES_FOUND)
    include_directories(${POSTGRES_INCLUDE_DIR})
    add_definitions(-DHAVE_PG)
endif()


set (HHEADERS
    ./include/cmd.h
    ./include/exports.h
    ./include/version.h
    ./include/dlg.h
)

set(CSOURCES
    ./src/cmd.cpp
    ./src/dlg.cpp
)

add_library(${LIB_NAME} SHARED ${HHEADERS} ${CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif()

if(WXGIS_FOUND)
   target_link_libraries(${LIB_NAME} ${WXGIS_LIBRARIES})
endif()

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif() 

if(UNIX)
    if(GTK2_FOUND)
        target_link_libraries(${LIB_NAME} ${GTK2_LIBRARIES})
    endif()
    
    if(GTK3_FOUND)
        target_link_libraries(${LIB_NAME} ${GTK3_LIBRARIES})
    endif()
    
    if(UNITY_FOUND)
        target_link_libraries(${LIB_NAME} ${UNITY_LIBRARIES})
    endif()
endif()

include(install)
include(uninstall)

