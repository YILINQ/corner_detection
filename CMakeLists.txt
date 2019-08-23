cmake_minimum_required(VERSION 2.8)

project( corner )

find_package( OpenCV REQUIRED )
INCLUDE_DIRECTORIES(/home/yilin/Desktop/Ecopia/env_libs/opencv-3.4.0)
add_executable( corner_main corner_main.cpp )

target_link_libraries( corner_main ${OpenCV_LIBS} )


