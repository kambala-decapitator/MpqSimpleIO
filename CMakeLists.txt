cmake_minimum_required(VERSION 3.12)
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum macOS version" FORCE)
if(WIN32)
  set(CMAKE_GENERATOR_PLATFORM "Win32" CACHE STRING "" FORCE) # force 32-bit build
endif()

project(MpqSimpleIO
  VERSION 1.0
  DESCRIPTION "Simple I/O for Diablo II MPQ files"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(STORMLIB StormLib)
add_library(${STORMLIB} STATIC IMPORTED)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} ${STORMLIB})

if(APPLE)
  set_target_properties(${STORMLIB} PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/StormLib/macos/libStormLib.a)
  target_link_libraries(${PROJECT_NAME} bz2 z)
elseif(WIN32)
  set(STORMLIB_DIR "${CMAKE_SOURCE_DIR}/StormLib/windows")
  set(STORMLIB_DEBUG   "${STORMLIB_DIR}/StormLibDAS.lib")
  set(STORMLIB_RELEASE "${STORMLIB_DIR}/StormLibRAS.lib")
  if(isMultiConfig)
    set_target_properties(${STORMLIB} PROPERTIES IMPORTED_LOCATION       "${STORMLIB_RELEASE}")
    set_target_properties(${STORMLIB} PROPERTIES IMPORTED_LOCATION_DEBUG "${STORMLIB_DEBUG}")
  else()
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	  set(STORMLIB_PATH STORMLIB_DEBUG)
	else()
	  set(STORMLIB_PATH STORMLIB_RELEASE)
	endif()
    set_target_properties(${STORMLIB} PROPERTIES IMPORTED_LOCATION "${${STORMLIB_PATH}}")
  endif()

  target_compile_definitions(${PROJECT_NAME} PRIVATE STORMLIB_NO_AUTO_LINK)
  target_compile_options(${PROJECT_NAME} PRIVATE /UUNICODE /U_UNICODE) # ANSI build
  target_compile_options(${PROJECT_NAME} PRIVATE $<IF:$<CONFIG:Debug>,/MTd,/MT>) # fix LNK4098, StormLib uses MT for static libs
endif()
