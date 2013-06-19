PROJECT(statefs-power-emu)
cmake_minimum_required(VERSION 2.8)

IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
  SET(LIB_SUFFIX "")
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 4)
  SET(LIB_SUFFIX 64)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)

set(DST_LIB lib${LIB_SUFFIX})

find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wall -O2 -g -feliminate-unused-debug-types -std=c++0x"
  )

pkg_check_modules(STATEFS statefs-cpp REQUIRED)

INCLUDE_DIRECTORIES(
  ${STATEFS_INCLUDES}
)

link_directories(
  ${STATEFS_LIBS}
)

add_subdirectory(src)

