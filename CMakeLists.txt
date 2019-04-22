cmake_minimum_required(VERSION 2.8)


if(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w")
else(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w -static")
endif(APPLE)

set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")


#LINK_DIRECTORIES(~/lib)
#include_directories(~/include)
#FILE(GLOB SOURCES ./src/*.cpp)
#include_directories(src)

add_executable(enumSubstring.out enumSubstring.cpp esa.hxx sais.hxx)
add_executable(enumMaximumSubstring.out enumMaximumSubstring.cpp esa.hxx sais.hxx)

#target_link_libraries(a.out divsufsort divsufsort64 sdsl)



