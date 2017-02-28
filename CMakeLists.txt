cmake_minimum_required(VERSION 2.8)
project(detection)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2 -pthread")
SET(CMAKE_CXX_FLAGS "-std=c++11 -O2 -pthread")
set(SYSROOT "/opt/hisi-linux/x86-arm/arm-hisiv400-linux/arm-hisiv400-linux-gnueabi")
LINK_DIRECTORIES( ${SYSROOT}/lib2 )
SET(CMAKE_LIBRARY_PATH ${SYSROOT}/lib2)
SET(CMAKE_SYSTEM_LIBRARY_PATH ${SYSROOT}/lib2)

include_directories(${SYSROOT}/include
                    ${URRENT_LIST_DIR}
                    ${CMAKE_CURRENT_LIST_DIR}/mtcnn )

set(LIBS pthread
         rt
         z
         openblas
         boost_system 
         boost_filesystem
         boost_thread
         glog
         protobuf
         hdf5_hl 
         hdf5
         gflags
         caffe # Illegal instruction error
         #libcaffe.a
         )

set(OPENCV_LIBS opencv_shape opencv_stitching opencv_objdetect opencv_superres opencv_videostab opencv_photo opencv_optflow opencv_tracking opencv_plot opencv_video opencv_ximgproc opencv_calib3d opencv_features2d opencv_highgui opencv_videoio opencv_imgcodecs opencv_ml opencv_imgproc opencv_flann opencv_core)

set(SRC
${CMAKE_CURRENT_LIST_DIR}/main.cpp 
${CMAKE_CURRENT_LIST_DIR}/motionproposal.cpp 
${CMAKE_CURRENT_LIST_DIR}/motionclassification.cpp 
${CMAKE_CURRENT_LIST_DIR}/motiondetection.cpp 
${CMAKE_CURRENT_LIST_DIR}/objectdetection.cpp 
${CMAKE_CURRENT_LIST_DIR}/objecttrack.cpp 
${CMAKE_CURRENT_LIST_DIR}/actionrecognition.cpp
${CMAKE_CURRENT_LIST_DIR}/mtcnn/caffeplus.cpp 
${CMAKE_CURRENT_LIST_DIR}/mtcnn/eigenplus.cpp 
${CMAKE_CURRENT_LIST_DIR}/mtcnn/nms.cpp 
${CMAKE_CURRENT_LIST_DIR}/mtcnn/mtcnn.cpp)

set(HEADER
${CMAKE_CURRENT_LIST_DIR}/common.h 
${CMAKE_CURRENT_LIST_DIR}/motionproposal.h 
${CMAKE_CURRENT_LIST_DIR}/motionclassification.h 
${CMAKE_CURRENT_LIST_DIR}/motiondetection.h 
${CMAKE_CURRENT_LIST_DIR}/objectdetection.h 
${CMAKE_CURRENT_LIST_DIR}/objecttrack.h 
${CMAKE_CURRENT_LIST_DIR}/actionrecognition.h
${CMAKE_CURRENT_LIST_DIR}/mtcnn/caffeplus.h 
${CMAKE_CURRENT_LIST_DIR}/mtcnn/eigenplus.h 
${CMAKE_CURRENT_LIST_DIR}/mtcnn/nms.h 
${CMAKE_CURRENT_LIST_DIR}/mtcnn/mtcnn.h)

add_definitions(-DCPU_ONLY -DUSE_OPENCV)
add_executable(detection ${SRC})
target_link_libraries(detection ${LIBS} ${OPENCV_LIBS})
