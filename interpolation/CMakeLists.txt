cmake_minimum_required(VERSION 3.10)
project(interpolation)

include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})
#add_library(${PROJECT_NAME}-core OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/upsample.h)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.cpp )
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

#Link to OpenCV library. 
find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

#add_executable(undistortImage undistortImage.cpp)
#target_link_libraries(undistortImage ${OpenCV_LIBS})
