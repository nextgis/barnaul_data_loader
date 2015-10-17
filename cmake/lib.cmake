project (${PROJECT_NAME})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)

if(WIN32)
  set(LIB_NAME ${PROJECT_NAME}${MAJOR_VERSION}${MINOR_VERSION})
else(WIN32)
  set(LIB_NAME ${PROJECT_NAME})
endif(WIN32)

message(STATUS "${PROJECT_NAME} lib name ${LIB_NAME}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/version_dll.h "//Copyright (C) 2015 NextGIS, info@nextgis.com\nBaryshnikov Dmitry, dmitry.baryshnikov@nextgis.ru\n#pragma once\n#define wxBDL_FILENAME \"${PROJECT_NAME}\" \n\n" )

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_UNICODE -DUNICODE -D_USRDLL)
  set(PROJECT_CSOURCES ${PROJECT_CSOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/version_dll.rc)
  source_group("Resource Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/version_dll.rc)  
endif(MSVC)

if(WIN32)
  add_definitions(-DWIN32 -D__WXMSW__ -D_WIN32_WINNT=0x0501)
endif(WIN32)

add_definitions(-DSTRICT)

if(MSVC OR MSVC_IDE)
    #string(REPLACE "/W4" "/W0" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    #string(REPLACE "/W4" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    # string(REPLACE "/Gm" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    #set(CMAKE_CXX_FLAGS_DEBUG "/Gm /ZI /W3 /Od")
    # add_definitions(-DDISABLE_SOME_FLOATING_POINT)
    # set_target_properties( ${the_target} PROPERTIES COMPILE_FLAGS "/Gm" )
    if(CMAKE_CL_64)
        set_target_properties(${the_target} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
        add_definitions(-D_WIN64)
    endif()
    
  if( MSVC_VERSION GREATER 1600 )
    set(CMAKE_GENERATOR_TOOLSET "v120_xp" CACHE STRING "Platform Toolset" FORCE) 
    add_definitions(-D_USING_V120_SDK71_)
  endif() 

endif()

if(UNIX)
     if(CMAKE_COMPILER_IS_GNUCXX OR CV_ICC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fno-strict-aliasing")#-Wextra -Wall -W -pthread -O2 -fno-strict-aliasing -pthrea
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-strict-aliasing")
     endif()
    # Select flags.
#    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O0 -g")
#    set(CMAKE_CXX_FLAGS_RELEASE        "-O")
#    set(CMAKE_CXX_FLAGS_DEBUG          "-Wall -g -O")
#    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O0 -g")
#    set(CMAKE_C_FLAGS_RELEASE          "-O")
#    set(CMAKE_C_FLAGS_DEBUG            "-Wall -g -O2")
endif()
