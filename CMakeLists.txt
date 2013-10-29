cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Possible configurations" FORCE)
if (DEFINED CMAKE_BUILD_TYPE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type")
endif()

project(filters C CXX)

find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(SEND_ERROR "Failed to find OpenCV")
    return()
else()
    include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
endif()

include_directories(library)
set(SOURCES main.cpp options.cpp filterFramework.cpp) 
add_executable(filter_framework ${SOURCES}) 

add_subdirectory(library)

target_link_libraries(filter_framework filter_library ${OpenCV_LIBRARIES})


