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

#set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG -DDEBUG_PRINT")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

#### Modules
set(MYMOD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/module)
set(MYMOD_BINARY_DIR ${CMAKE_BINARY_DIR}/module)

if(COMPILE_LIBDIVSUFSORT)
add_subdirectory(${MYMOD_SOURCE_DIR}/libdivsufsort ${MYMOD_BINARY_DIR}/libdivsufsort)
include_directories(${MYMOD_BINARY_DIR}/libdivsufsort/include)
else()
#libdivsufsort directory is here.
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)
INCLUDE_DIRECTORIES(~/include)
endif()

add_subdirectory(${MYMOD_SOURCE_DIR}/Basics ${MYMOD_BINARY_DIR}/Basics)
add_subdirectory(${MYMOD_SOURCE_DIR}/BTree ${MYMOD_BINARY_DIR}/BTree)
add_subdirectory(${MYMOD_SOURCE_DIR}/OnlineRlbwt ${MYMOD_BINARY_DIR}/OnlineRlbwt)

#SDSL lib and include directory are here
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)
include_directories(~/include)


#set(GLOB SOURCES src/minimal_substrings/*.cpp)

# STOOL submodule
FILE(GLOB STOOL_SOURCES module/stool/src/*.cpp)
INCLUDE_DIRECTORIES(module)


add_executable(enumSubstring.out ${STOOL_SOURCES} src/main/enumSubstring.cpp)
target_link_libraries(enumSubstring.out divsufsort divsufsort64)

add_executable(enumMaximalSubstring.out ${STOOL_SOURCES} src/main/enumMaximalSubstring.cpp)
target_link_libraries(enumMaximalSubstring.out sdsl divsufsort divsufsort64  online_rlbwt Basics BTree)

add_executable(enumKaiMaximalSubstring.out ${STOOL_SOURCES} src/main/enumKaiMaximalSubstring.cpp)
target_link_libraries(enumKaiMaximalSubstring.out divsufsort divsufsort64)

add_executable(enumMinimalSubstring.out ${STOOL_SOURCES} src/main/enumMinimalSubstring.cpp )
target_link_libraries(enumMinimalSubstring.out divsufsort divsufsort64)

add_executable(test.out ${STOOL_SOURCES} src/main/test_main.cpp )
target_link_libraries(test.out divsufsort divsufsort64)



add_executable(print.out ${STOOL_SOURCES} src/main/print.cpp)
target_link_libraries(print.out divsufsort divsufsort64)

add_executable(sdsl.out ${STOOL_SOURCES} src/main/sdsl_main.cpp)
target_link_libraries(sdsl.out sdsl divsufsort divsufsort64)

add_executable(test2.out ${STOOL_SOURCES} src/main/test2_main.cpp )
target_link_libraries(test2.out sdsl divsufsort divsufsort64 online_rlbwt Basics BTree)




