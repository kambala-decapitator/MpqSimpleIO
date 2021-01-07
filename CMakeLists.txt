cmake_minimum_required(VERSION 3.19)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum macOS version" FORCE)

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
  set(STORMLIB_NAME $<IF:$<CONFIG:Debug>,StormLibDAS,StormLibRAS>)
  set_target_properties(${STORMLIB} PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/StormLib/windows/${STORMLIB_NAME}.lib")

  target_compile_definitions(${PROJECT_NAME} PRIVATE STORMLIB_NO_AUTO_LINK)
  target_compile_options(${PROJECT_NAME} PRIVATE /UUNICODE /U_UNICODE) # ANSI build
  target_compile_options(${PROJECT_NAME} PRIVATE $<IF:$<CONFIG:Debug>,/MTd,/MT>) # fix LNK4098, StormLib uses MT for static libs
endif()
