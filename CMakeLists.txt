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
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(./include)

add_executable(enumSubstring.out enumSubstring.cpp esa.hxx mycode.hpp)
target_link_libraries(enumSubstring.out divsufsort divsufsort64)

add_executable(enumMaximalSubstring.out enumMaximalSubstring.cpp esa.hxx mycode.hpp postorder_maximal_substrings.hpp)
target_link_libraries(enumMaximalSubstring.out divsufsort divsufsort64)

add_executable(enumKaiMaximalSubstring.out enumKaiMaximalSubstring.cpp esa.hxx mycode.hpp postorder_maximal_substrings.hpp)
target_link_libraries(enumKaiMaximalSubstring.out divsufsort divsufsort64)


add_executable(print.out print.cpp mycode.hpp)
target_link_libraries(print.out divsufsort divsufsort64)





