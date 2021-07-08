cmake_minimum_required(VERSION 3.5)

project(cmake-macro
        VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(STATIC_HEADER "include/static/")
#add_library(aLib STATIC src/lib.cpp ${STATIC_HEADER}lib.hpp)

#target_include_directories(aLib
#    INTERFACE
#        include/
#)

#set(SOURCES
#    main.cpp
#)

include(${SECOND_OS_BRAND}.cmake)

if(DEFINED SECOND_OS_BRAND)
    # same like: g++ main.cpp -o demo -DXXX
    message(STATUS ${SECOND_OS_BRAND})
    add_definitions(-D${SECOND_OS_BRAND})
endif()

message(STATUS ${SOURCES})

add_executable(demo ${SOURCES})

#target_link_libraries(demo
#    PRIVATE
#        aLib
#)
