cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME "frobel" )

project (${PROJECT_NAME})

set(CMAKE_MODULE_PATH "/opt/opencv331/opencv")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" )
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src" )

#Compilers
include( CheckCXXCompilerFlag )
CHECK_CXX_COMPILER_FLAG( "-std=c++11" COMPILER_SUPPORTS_CXX11 )
CHECK_CXX_COMPILER_FLAG( "-std=c++0x" COMPILER_SUPPORTS_CXX0X )

if( COMPILER_SUPPORTS_CXX11 )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
elseif( COMPILER_SUPPORTS_CXX0X )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )
else()
    message( STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler." )
endif()
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w" ) # to surpress warnings

#OpenCv
find_package(OpenCV QUIET)
if(NOT OpenCV_FOUND)
  message("OpenCV example will not be build as no OpenCV was found!")
  return()
endif()

# Includes
include_directories(
  ${INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
)

# Project Files
set(
  HEADERS
  Detector.h 
  Calibrator.h 
  Util.h 
  Common.h
)

set(
  SOURCES 
  Detector.cpp 
  Calibrator.cpp  
  Util.cpp  
)

add_executable(${PROJECT_NAME} ${SOURCES} main3.cpp)
target_link_libraries(${PROJECT_NAME} "${OpenCV_LIBRARIES}") 
 



