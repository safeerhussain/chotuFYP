cmake_minimum_required(VERSION 2.8.3)
project(mypackage)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs kobuki_msgs sensor_msgs cv_bridge roscpp std_msgs image_transport)
find_package( OpenCV REQUIRED )
find_package( PCL REQUIRED )

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/groovy/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

#######################################
## Declare ROS messages and services ##
#######################################

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#   INCLUDE_DIRS include
#  LIBRARIES mypackage
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations


include_directories(  ${catkin_INCLUDE_DIRS}  ${OpenCV_INCLUDE_DIRS} ${PCL_INCLUDE_DIRS}  )
## Declare a cpp library
# add_library(mypackage
#   src/${PROJECT_NAME}/mypackage.cpp
# )

## Declare a cpp executable
# add_executable(mypackage_node src/mypackage_node.cpp)
add_executable(move src/move_cpp.cpp)
add_executable(bump src/bump_cpp.cpp)
add_executable(depth src/depth.cpp)
add_executable(button src/button_cpp.cpp)
add_executable(image src/image_cpp.cpp)
add_executable(socket1 src/socket1.cpp)
add_executable(face src/face_cpp.cpp)
add_executable(socket2 src/socket2.cpp)
add_executable(sift src/sift_cpp.cpp)

## Add cmake target dependencies of the executable/library
## as an example, message headers may need to be generated before nodes
add_dependencies(move mypackage_generate_messages_cpp)
add_dependencies(bump mypackage_generate_messages_cpp)
add_dependencies(depth mypackage_generate_messages_cpp)
add_dependencies(button mypackage_generate_messages_cpp)
add_dependencies(image mypackage_generate_messages_cpp)
add_dependencies(socket1 mypackage_generate_messages_cpp)
add_dependencies(face mypackage_generate_messages_cpp)
add_dependencies(socket2 mypackage_generate_messages_cpp)
add_dependencies(sift mypackage_generate_messages_cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(move ${catkin_LIBRARIES})
target_link_libraries(button ${catkin_LIBRARIES})
target_link_libraries (bump ${OpenCV_LIBRARIES} ${catkin_LIBRARIES} ${PCL_LIBRARIES} )
target_link_libraries (depth ${OpenCV_LIBRARIES} ${catkin_LIBRARIES} ${PCL_LIBRARIES} )
target_link_libraries (image ${OpenCV_LIBRARIES} ${catkin_LIBRARIES} ${PCL_LIBRARIES} )
target_link_libraries (socket1 ${OpenCV_LIBRARIES} ${catkin_LIBRARIES} ${PCL_LIBRARIES} )
target_link_libraries (face ${OpenCV_LIBRARIES} ${catkin_LIBRARIES} ${PCL_LIBRARIES} )
target_link_libraries (socket2 ${OpenCV_LIBRARIES} ${catkin_LIBRARIES} ${PCL_LIBRARIES} )
target_link_libraries (sift ${OpenCV_LIBRARIES} ${catkin_LIBRARIES} ${PCL_LIBRARIES} ) 

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/groovy/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
# install(TARGETS mypackage mypackage_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_mypackage.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
