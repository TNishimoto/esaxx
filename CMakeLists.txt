cmake_minimum_required(VERSION 2.8)


if(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w")
else(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w -static")
endif(APPLE)

set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

#libdivsufsort directory is here.
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)

add_executable(enumSubstring.out enumSubstring.cpp esa.hxx mycode.hpp divsufsort.h divsufsort64.h)
target_link_libraries(enumSubstring.out divsufsort divsufsort64)

add_executable(enumMaximalSubstring.out enumMaximalSubstring.cpp esa.hxx mycode.hpp divsufsort.h divsufsort64.h)
target_link_libraries(enumMaximalSubstring.out divsufsort divsufsort64)

#add_executable(enumLimitedMS.out enumLimitedMS.cpp esa.hxx mycode.hpp divsufsort.h divsufsort64.h)
#target_link_libraries(enumLimitedMS.out divsufsort divsufsort64)

#add_executable(intv.out intv.cpp esa.hxx mycode.hpp divsufsort.h divsufsort64.h)
#target_link_libraries(intv.out divsufsort divsufsort64)


add_executable(print.out print.cpp mycode.hpp)
target_link_libraries(print.out divsufsort divsufsort64)





