cmake_minimum_required(VERSION 2.8)
project(esaxx CXX)

#if(APPLE)
#set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w")
#else(APPLE)
#set(CMAKE_CXX_FLAGS "-std=c++11  -m64 -w -static")
#endif(APPLE)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++11 -m64 -Wall -pthread -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++11 -m64 -Wall -pthread -Wextra")
endif(CPP_STATIC)

#set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG -DDEBUG_PRINT")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

#set(ROOT_PROJECT  "esaxx")
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    message("Release Mode")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Set PROJECT_IS_TOP_LEVEL to ON if if this is the top level project (not
# if this is included as a subproject of something else). Note that this is
# handled automatically for CMake >= 3.21.
if (CMAKE_VERSION VERSION_LESS 3.21)
    if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
        message("SET PROJECT_IS_TOP_LEVEL ON IN ${PROJECT_NAME}")
        set (PROJECT_IS_TOP_LEVEL ON)
    else()
        message("SET PROJECT_IS_TOP_LEVEL OFF IN ${PROJECT_NAME}")
    set (PROJECT_IS_TOP_LEVEL OFF)    
    endif()
endif ()

#### Modules

if(NOT TARGET stool)
add_subdirectory(module/stool)
endif()

#set(MYMOD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/module)
#set(MYMOD_BINARY_DIR ${CMAKE_BINARY_DIR}/module)
#set(MYMOD_DIR 1)

#add_subdirectory(${MYMOD_SOURCE_DIR}/libdivsufsort ${MYMOD_BINARY_DIR}/libdivsufsort)
#include_directories(${MYMOD_BINARY_DIR}/libdivsufsort/include)

#libdivsufsort directory is here.
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)
INCLUDE_DIRECTORIES(~/include)

#SDSL lib and include directory are here
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)
include_directories(~/include)


#set(GLOB SOURCES src/minimal_substrings/*.cpp)

# STOOL submodule
FILE(GLOB STOOL_SOURCES module/stool/lib/*.cpp module/libdivsufsort/sa.cpp)
#FILE(GLOB SOURCES lib/debug/*.cpp)

INCLUDE_DIRECTORIES(module)



if (PROJECT_IS_TOP_LEVEL)
message("TOP LEVEL IS ${PROJECT_NAME}")

add_library(fn2 STATIC ${STOOL_SOURCES})
target_link_libraries(fn2 sdsl divsufsort divsufsort64)


add_executable(enumMaximalSubstring.out main/enumMaximalSubstring.cpp)
target_link_libraries(enumMaximalSubstring.out fn2)

add_executable(enumMaximalSubstring_input_bwt.out main/enumMaximalSubstring_input_bwt.cpp)
target_link_libraries(enumMaximalSubstring_input_bwt.out fn2)

add_executable(test.out main/minimal_substring_test_main.cpp)
target_link_libraries(test.out fn2)


else()
#add_library(esaxx STATIC ${SOURCES})
#target_link_libraries(esaxx sdsl divsufsort divsufsort64 stool)
endif()







#add_executable(enumLineMaximalSubstring.out src/main/enumLineMaximalSubstring.cpp )
#target_link_libraries(enumLineMaximalSubstring.out fn)
#add_executable(enumMaximalSubstringWithRLBWT.out src/main/enumMaximalSubstringWithRLBWT.cpp )
#target_link_libraries(enumMaximalSubstringWithRLBWT.out fn)
#
#add_executable(print.out src/main/print.cpp)
#target_link_libraries(print.out fn)
#
#
#add_executable(lisInLcpArray.out src/debug/lis_in_lcp_array.cpp )
#target_link_libraries(lisInLcpArray.out fn)
#add_executable(online_rlbwt.out ${MYMOD_SOURCE_DIR}/rlbwt_iterator/src/main/compress_main.cpp)
#target_link_libraries(online_rlbwt.out fn)


### Debug
#add_executable(beller.out src/main/beller_main.cpp)
#target_link_libraries(beller.out fn)
#
#add_executable(hyper.out src/main/hyper_main.cpp)
#target_link_libraries(hyper.out fn)



#add_executable(rlesa.out src/debug/rlesa_main.cpp )
#target_link_libraries(rlesa.out fn)

#add_executable(enumMinimalSubstring.out src/main/enumMinimalSubstring.cpp )
#target_link_libraries(enumMinimalSubstring.out fn)

#add_executable(enumKaiMaximalSubstring.out src/main/enumKaiMaximalSubstring.cpp)
#target_link_libraries(enumKaiMaximalSubstring.out fn)




#add_executable(test.out src/debug/test_main.cpp )
#target_link_libraries(test.out fn)

#add_executable(sdsl.out src/debug/sdsl_main.cpp)
#target_link_libraries(sdsl.out fn)

#add_executable(interval_test.out src/debug/interval_test_main.cpp )
#target_link_libraries(interval_test.out fn)

#add_executable(depth.out src/debug/depth_main.cpp )
#target_link_libraries(depth.out fn)

#add_executable(array_test.out src/debug/array_test_main.cpp )
#target_link_libraries(array_test.out fn)

#add_executable(lcp_interval_test.out src/debug/lcp_interval_test_main.cpp )
#target_link_libraries(lcp_interval_test.out fn)



