project(wxWidgetTest)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(src SRC_LIST)
aux_source_directory(src/gui SRC_LIST)
aux_source_directory(src/camera SRC_LIST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wall -Werror -pedantic -g")

set(wxWidgets_CONFIGURATION wxOSX/Cocoa)
find_package(wxWidgets COMPONENTS core base REQUIRED)

find_package(OpenCV REQUIRED)

include(${wxWidgets_USE_FILE})
 
include_directories(SYSTEM ${OpenCV_INC})

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${OpenCV_LIBS})