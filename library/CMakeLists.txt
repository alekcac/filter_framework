cmake_minimum_required(VERSION 2.8.7)

project(filter_library)

set(SOURCE_LIB filters.cpp)
add_library(filter_library STATIC ${SOURCE_LIB})
target_link_libraries(filter_library ${OpenCV_LIBRARIES})