cmake_minimum_required(VERSION 3.8)
project(NetworkCamera)
# install dir
set(CMAKE_INSTALL_PREFIX ${SOLUTION_DIR}/build CACHE STRING "set install prefix" FORCE)

set(CMAKE_CXX_STANDARD 11)
# set base flags
set(CMAKE_CFLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -std=c++11")


add_definitions(-DNETWORKCAMERA_EXPORT)

include_directories(/usr/local/boost-1.60/include)
include_directories(/usr/local/OpenCV2.4.13/include)
include_directories(/usr/local/libvlc/include)
include_directories(/usr/local/libvlc/include/vlc)
include_directories(/usr/local/libvlcpp)

set(CMAKE_INCLUDE_CURRENT_DIR ".")

file(GLOB MY_SRC_FILES
        ${CMAKE_INCLUDE_CURRENT_DIR}/src/*.cpp
        ${CMAKE_INCLUDE_CURRENT_DIR}/src/*.cc
        ${CMAKE_INCLUDE_CURRENT_DIR}/src/*.c
        )
file(GLOB INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        )

list(APPEND SOURCE_FILES ${MY_SRC_FILES})
list(APPEND SOURCE_FILES ${INCLUDE_FILES})


link_directories(
        /usr/local/boost-1.60/lib
        /usr/local/OpenCV2.4.13/lib
        /usr/local/libvlc/lib
)



add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} -lopencv_highgui
        -lopencv_imgproc
        -lopencv_core
        -lvlccore
        -lvlc
        -lboost_date_time)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

