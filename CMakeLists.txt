cmake_minimum_required(VERSION 2.8.3)
project(uwesub_svs)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS 
	roscpp
	std_msgs
	geometry_msgs
	message_generation
	)



## Declare a catkin package
catkin_package(
	CATKIN_DEPENDS 
	roscpp
	std_msgs
	geometry_msgs
	INCLUDE_DIRS
	include
	 )

add_library(${PROJECT_NAME} src/serial_port.cpp )

## Build talker and listener
include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(svs src/svs.cpp)
add_library(variance_tool src/calibrate_covariance.cpp )

target_link_libraries(svs ${catkin_LIBRARIES} ${PROJECT_NAME} variance_tool)
add_dependencies(svs std_msgs_generate_messages_cpp geometry_msgs_generate_messages_cpp)
