# - Try to find the WXGIS libraries
# Once done this will define
#
#  WXGIS_ROOT_DIR - Set this variable to the root installation of WXGISGDAL
#
# Read-Only variables:
#  WXGIS_FOUND - system has the WXGISGDAL library
#  WXGIS_INCLUDE_DIR - the WXGISGDAL include directory
#  WXGIS_LIBRARIES - The libraries needed to use WXGISGDAL
#  WXGIS_VERSION - This is set to $major.$minor.$revision (eg. 0.5.0)

#=============================================================================
# Copyright 2012,2015 Dmitry Baryshnikov <polimax at mail dot ru>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)
MACRO(DBG_MSG _MSG)
  MESSAGE(STATUS
    "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${_MSG}")
ENDMACRO(DBG_MSG)
MACRO(DBG_MSG_V _MSG)
  MESSAGE(STATUS
    "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${_MSG}")
ENDMACRO(DBG_MSG_V)
  
if (UNIX)
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(_WXGIS wxgis)
  endif (PKG_CONFIG_FOUND)
endif (UNIX)

SET(_WXGIS_ROOT_HINTS
  $ENV{wxGIS_ROOT_DIR}
  $ENV{WXGIS}  
  ${WXGIS_ROOT_DIR}
  )
  
SET(_WXGIS_ROOT_PATHS
  $ENV{wxGIS_ROOT_DIR}
  $ENV{WXGIS}
  /usr/include
  /usr/local/include
  /usr/lib/
  /usr/local/lib
  )
SET(_WXGIS_ROOT_HINTS_AND_PATHS
  HINTS ${_WXGIS_ROOT_HINTS}
  PATHS ${_WXGIS_ROOT_PATHS}
  )

FIND_PATH(WXGIS_INCLUDE_DIR
  NAMES
    wxgis/version.h
  HINTS
    ${_WXGIS_INCLUDEDIR}
    ${_WXGIS_ROOT_HINTS_AND_PATHS}
  PATH_SUFFIXES
    include
)

if (WXGIS_INCLUDE_DIR) 
     file(READ "${WXGIS_INCLUDE_DIR}/wxgis/version.h" WXGIS_VERSION_H_CONTENTS)
    string(REGEX MATCH "wxGIS_MAJOR_VERSION[ \t]+([0-9]+)"
      wxGIS_MAJOR_VERSION ${WXGIS_VERSION_H_CONTENTS})
    string (REGEX MATCH "([0-9]+)"
      wxGIS_MAJOR_VERSION ${wxGIS_MAJOR_VERSION})
    string(REGEX MATCH "wxGIS_MINOR_VERSION[ \t]+([0-9]+)"
      wxGIS_MINOR_VERSION ${WXGIS_VERSION_H_CONTENTS})
    string (REGEX MATCH "([0-9]+)"
      wxGIS_MINOR_VERSION ${wxGIS_MINOR_VERSION})
    string(REGEX MATCH "wxGIS_RELEASE_NUMBER[ \t]+([0-9]+)"
      wxGIS_RELEASE_NUMBER ${WXGIS_VERSION_H_CONTENTS})  
    string (REGEX MATCH "([0-9]+)"
      wxGIS_RELEASE_NUMBER ${wxGIS_RELEASE_NUMBER})
        
    # Setup package meta-data
    set(WXGIS_VERSION ${wxGIS_MAJOR_VERSION}.${wxGIS_MINOR_VERSION} CACHE INTERNAL "The version number for wxgis libraries")

endif (WXGIS_INCLUDE_DIR)

IF(MSVC)
    set(P_SUFF "lib" "lib/Release" "lib/Debug")
    if(CMAKE_CL_64)
        set(P_SUFF ${P_SUFF} "lib/x64")
    else() 
        set(P_SUFF ${P_SUFF} "lib/x86")    
    endif()
ELSE(MSVC)
    set(P_SUFF "lib" "lib/wxgis")  
ENDIF(MSVC)    

FIND_PATH(WXGIS_LIB_DIR
    NAMES
      wxgiscore${wxGIS_MAJOR_VERSION}${wxGIS_MINOR_VERSION}d.dll   
      wxgiscore${wxGIS_MAJOR_VERSION}${wxGIS_MINOR_VERSION}.dll      
      libwxgiscored.so   
      libwxgiscore.so          
    PATHS 
    ${_WXGIS_ROOT_PATHS}
    PATH_SUFFIXES
    ${P_SUFF}
    DOC "Path to wxGIS libraries?"
    NO_DEFAULT_PATH
    )
    
IF(NOT WXGIS_FIND_COMPONENTS)
  SET(WXGIS_FIND_COMPONENTS core)
ENDIF(NOT WXGIS_FIND_COMPONENTS)  
DBG_MSG("wxGIS_FIND_COMPONENTS : ${WXGIS_FIND_COMPONENTS}")  
    
FOREACH(LIB ${WXGIS_FIND_COMPONENTS})
  FIND_LIBRARY(WXGIS_${LIB}
    NAMES
    wxgis${LIB}${wxGIS_MAJOR_VERSION}${wxGIS_MINOR_VERSION}
    wxgis${LIB}
    PATHS ${WXGIS_LIB_DIR}
    NO_DEFAULT_PATH
    )
  
  FIND_LIBRARY(WXGIS_${LIB}d
    NAMES
    wxgis${LIB}${wxGIS_MAJOR_VERSION}${wxGIS_MINOR_VERSION}d
    wxgis${LIB}d
    PATHS ${WXGIS_LIB_DIR}
    NO_DEFAULT_PATH
    )
  
    if(NOT WXGIS_${LIB}d AND WXGIS_${LIB})
        set(WXGIS_${LIB}d ${WXGIS_${LIB}} CACHE INTERNAL "component ${LIB} name")	
    endif()

    if(NOT WXGIS_${LIB} AND WXGIS_${LIB}d)
        set(WXGIS_${LIB} ${WXGIS_${LIB}d} CACHE INTERNAL "component ${LIB} name")	
    endif()
  
    MARK_AS_ADVANCED(WXGIS_${LIB})
    MARK_AS_ADVANCED(WXGIS_${LIB}d)  
  
ENDFOREACH()    

FOREACH(LIB ${WXGIS_FIND_COMPONENTS})
    DBG_MSG_V("Searching for ${LIB} and ${LIB}d")
    DBG_MSG_V("WXGIS_${LIB}  : ${WXGIS_${LIB}}")
    DBG_MSG_V("WXGIS_${LIB}d : ${WXGIS_${LIB}d}")
    IF(WXGIS_${LIB} AND WXGIS_${LIB}d)
      DBG_MSG_V("Found ${LIB} and ${LIB}d")
      LIST(APPEND WXGIS_LIBRARIES
        debug ${WXGIS_${LIB}d} optimized ${WXGIS_${LIB}}
        )
    ELSE(WXGIS_${LIB} AND WXGIS_${LIB}d)
      DBG_MSG_V("- not found due to missing WXGIS_${LIB}=${WXGIS_${LIB}} or WXGIS_${LIB}d=${WXGIS_${LIB}d}")
      SET(WXGIS_FOUND FALSE)
    ENDIF(WXGIS_${LIB} AND WXGIS_${LIB}d)
ENDFOREACH(LIB)


# IF(WIN32 AND NOT CYGWIN)
  # # MINGW should go here too
  # IF(MSVC)
    # set(P_SUFF "lib" "lib/Release" "lib/Debug")
    # if(CMAKE_CL_64)
        # set(P_SUFF ${P_SUFF} "lib/x64")
    # else() 
        # set(P_SUFF ${P_SUFF} "lib/x86")    
    # endif()
    # # Implementation details:
    # # We are using the libraries located in the VC subdir instead of the parent directory eventhough :
    # FIND_LIBRARY(WXGISCARTO_DEBUG
      # NAMES
        # wxgiscarto04d
      # ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
      # PATH_SUFFIXES
        # ${P_SUFF}
    # )

    # FIND_LIBRARY(WXGISCARTO_RELEASE
      # NAMES
        # wxgiscarto04
      # ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
      # PATH_SUFFIXES
        # ${P_SUFF}
    # )

    # if( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
        # if(NOT WXGIS_DEBUG)
            # set(WXGIS_DEBUG ${WXGIS_RELEASE})
        # endif(NOT WXGIS_DEBUG)    
      # set( WXGIS_LIBRARIES
        # optimized ${WXGIS_RELEASE} debug ${WXGIS_DEBUG}
        # )
    # else()
      # set( WXGIS_LIBRARIES ${WXGIS_RELEASE})
    # endif()
    # MARK_AS_ADVANCED(WXGIS_DEBUG WXGIS_RELEASE)
  # ELSEIF(MINGW)
    # # same player, for MingW
    # FIND_LIBRARY(WXGISCARTO
      # NAMES        
        # wxgiscarto04
      # ${_WXGISGDAL_ROOT_HINTS_AND_PATHS}
      # PATH_SUFFIXES
        # "lib"
        # "lib/MinGW"
    # )

    # MARK_AS_ADVANCED(WXGISCARTO)
    # set( WXGIS_LIBRARIES ${WXGISCARTO})
  # ELSE(MSVC)
    # # Not sure what to pick for -say- intel, let's use the toplevel ones and hope someone report issues:
    # FIND_LIBRARY(WXGISCARTO
      # NAMES
        # wxgiscarto04
      # HINTS
        # ${_WXGIS_LIBDIR}
      # ${_WXGIS_ROOT_HINTS_AND_PATHS}
      # PATH_SUFFIXES
        # lib
    # ) 

    # MARK_AS_ADVANCED(WXGISCARTO)
    # set( WXGIS_LIBRARIES ${WXGISCARTO} )
  # ENDIF(MSVC)
# ELSE(WIN32 AND NOT CYGWIN)

  # FIND_LIBRARY(WXGISCARTO
    # NAMES
        # wxgiscarto04
    # HINTS
      # ${_WXGIS_LIBDIR}
    # ${_WXGIS_ROOT_HINTS_AND_PATHS}
    # PATH_SUFFIXES
      # lib
  # ) 

  # MARK_AS_ADVANCED(WXGIS_LIBRARY)

  # # compat defines
  # SET(WXGIS_LIBRARIES ${WXGIS_LIBRARY})

# ENDIF(WIN32 AND NOT CYGWIN)

#SET(WXGIS_LIBRARIES "qqq")#hack

include(FindPackageHandleStandardArgs)

if (WXGIS_VERSION)
  find_package_handle_standard_args(WXGIS
    REQUIRED_VARS
      WXGIS_LIBRARIES
      WXGIS_INCLUDE_DIR
    VERSION_VAR
      WXGIS_VERSION
    FAIL_MESSAGE
      "Could NOT find WXGIS, try to set the path to WXGIS root folder in the system variable WXGIS"
  )
else (WXGIS_VERSION)
  find_package_handle_standard_args(WXGIS "Could NOT find WXGIS, try to set the path to WXGIS root folder in the system variable WXGIS"
    WXGIS_LIBRARIES
    WXGIS_INCLUDE_DIR
  )
endif (WXGIS_VERSION)

MARK_AS_ADVANCED(WXGIS_INCLUDE_DIR WXGIS_LIBRARIES)
