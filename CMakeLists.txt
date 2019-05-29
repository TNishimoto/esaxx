cmake_minimum_required(VERSION 2.8)


if(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w")
else(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w -static")
endif(APPLE)

set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

#### Modules
if(NOT MYMOD_DIR)
  set(MYMOD_DIR 1)
  set(MYMOD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/module)
  set(MYMOD_BINARY_DIR ${CMAKE_BINARY_DIR}/module)
  add_subdirectory(${MYMOD_SOURCE_DIR}/libdivsufsort ${MYMOD_BINARY_DIR}/libdivsufsort)
endif()
include_directories(${MYMOD_BINARY_DIR}/libdivsufsort/include)

#libdivsufsort directory is here.
#Please modify it appropriately.
#LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)
#INCLUDE_DIRECTORIES(./include)

add_executable(enumSubstring.out enumSubstring.cpp esa.hxx mycode.hpp)
target_link_libraries(enumSubstring.out divsufsort divsufsort64)

add_executable(enumMaximalSubstring.out enumMaximalSubstring.cpp esa.hxx mycode.hpp postorder_maximal_substrings.hpp)
target_link_libraries(enumMaximalSubstring.out divsufsort divsufsort64)

add_executable(enumKaiMaximalSubstring.out enumKaiMaximalSubstring.cpp esa.hxx mycode.hpp postorder_maximal_substrings.hpp)
target_link_libraries(enumKaiMaximalSubstring.out divsufsort divsufsort64)


add_executable(print.out print.cpp mycode.hpp)
target_link_libraries(print.out divsufsort divsufsort64)





