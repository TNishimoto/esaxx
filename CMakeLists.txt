cmake_minimum_required(VERSION 2.8)


#if(APPLE)
#set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w")
#else(APPLE)
#set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w -static")
#endif(APPLE)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra")
endif(CPP_STATIC)

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

add_executable(enumSubstring.out src/enumSubstring.cpp)
target_link_libraries(enumSubstring.out divsufsort divsufsort64)

add_executable(enumMaximalSubstring.out src/enumMaximalSubstring.cpp)
target_link_libraries(enumMaximalSubstring.out divsufsort divsufsort64)

add_executable(enumKaiMaximalSubstring.out src/enumKaiMaximalSubstring.cpp)
target_link_libraries(enumKaiMaximalSubstring.out divsufsort divsufsort64)

add_executable(enumMinimalSubstring.out src/enumMinimalSubstring.cpp)
target_link_libraries(enumMinimalSubstring.out divsufsort divsufsort64)



add_executable(print.out src/print.cpp)
target_link_libraries(print.out divsufsort divsufsort64)





