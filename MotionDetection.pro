TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
TARGET = MotionDetection
INCLUDEPATH += .
DEFINES += USE_OPENCV CPU_ONLY

contains(TEMPLATE, app){ SOURCES += main.cpp }
#contains(TEMPLATE, lib) {
#SOURCES += interface.cpp
#HEADERS += interface.h
#}

SOURCES += \
    motionproposal.cpp \
    motionclassification.cpp \
    objectdetection.cpp \
    objecttrack.cpp \
    motiondetection.cpp \
    actionrecognition.cpp

HEADERS += \
    motionproposal.h \
    motionclassification.h \
    objectdetection.h \
    objecttrack.h \
    motiondetection.h \
    common.h \
    actionrecognition.h

win32{ HEADERS += headlayer.h}

HEADERS += \
    mtcnn/caffeplus.h \
    mtcnn/eigenplus.h \
    mtcnn/nms.h \
    mtcnn/mtcnn.h
SOURCES += \
    mtcnn/caffeplus.cpp \
    mtcnn/eigenplus.cpp \
    mtcnn/nms.cpp \
    mtcnn/mtcnn.cpp

#configuration variables

unix:!macx{
PATH_OPENCV_INCLUDE   = "/usr/local/include"
PATH_OPENCV_LIBRARIES = "/usr/local/lib"
VERSION_OPENCV        = 310
#include and build
INCLUDEPATH += $${PATH_OPENCV_INCLUDE}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_core
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_highgui
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgcodecs
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_videoio
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_video
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgproc
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_optflow
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_tracking
# glog, boost, protobuf, eigen, libigl
LIBS += -lglog -lprotobuf
LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_system
INCLUDEPATH += /usr/local/include/eigen
INCLUDEPATH += /usr/local/include/libigl/include
# caffe
INCLUDEPATH += /home/duino/src/caffe/include
LIBS += -L/home/duino/src/caffe/build/lib -lcaffe
}



win32{
LIBS_PATH = E:\debug\caffe_environment_x86_x64
ARCH = x86 #x64

# opencv
PATH_OPENCV_INCLUDE   = $${LIBS_PATH}\OpenCV\opencv310\build\include
PATH_OPENCV_LIBRARIES = $${LIBS_PATH}\OpenCV\opencv310\build\\$${ARCH}\vc12\lib
VERSION_OPENCV        = 310
INCLUDEPATH += $${PATH_OPENCV_INCLUDE}
CONFIG(debug, debug|release){
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_core$${VERSION_OPENCV}d
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_highgui$${VERSION_OPENCV}d
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgcodecs$${VERSION_OPENCV}d
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgproc$${VERSION_OPENCV}d
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_video$${VERSION_OPENCV}d
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_videoio$${VERSION_OPENCV}d
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_optflow$${VERSION_OPENCV}d
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_tracking$${VERSION_OPENCV}d
}
CONFIG(release, debug|release){
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_core$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_highgui$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgcodecs$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgproc$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_video$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_videoio$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_optflow$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_tracking$${VERSION_OPENCV}
}
#boost
INCLUDEPATH += $${LIBS_PATH}\boost\boost_1_59_0\include
CONFIG(debug, debug|release): BOOST_VERSION = "-vc120-mt-gd-1_59"
CONFIG(release, debug|release): BOOST_VERSION = "-vc120-mt-1_59"
LIBS += -L$${LIBS_PATH}\boost\boost_1_59_0\lib\vc120\\$${ARCH} \
    -llibboost_system$${BOOST_VERSION} \
    -llibboost_date_time$${BOOST_VERSION} \
    -llibboost_filesystem$${BOOST_VERSION} \
    -llibboost_thread$${BOOST_VERSION} \
    -llibboost_regex$${BOOST_VERSION}
#glog
INCLUDEPATH += $${LIBS_PATH}\glog\include
LIBS += -L$${LIBS_PATH}\glog\lib\\$${ARCH}\v120\Release\dynamic -llibglog
#gflags
INCLUDEPATH += $${LIBS_PATH}\gflags\include
CONFIG(debug, debug|release):   LIBS += -L$${LIBS_PATH}\gflags\\$${ARCH}\v120\dynamic\Lib -lgflagsd
CONFIG(release, debug|release): LIBS += -L$${LIBS_PATH}\gflags\\$${ARCH}\v120\dynamic\Lib -lgflags
#protobuf
INCLUDEPATH += $${LIBS_PATH}\protobuf\include
CONFIG(debug, debug|release):   LIBS += -L$${LIBS_PATH}\protobuf\lib\\$${ARCH}\v120\Debug -llibprotobuf
CONFIG(release, debug|release): LIBS += -L$${LIBS_PATH}\protobuf\lib\\$${ARCH}\v120\Release -llibprotobuf
# hdf5
INCLUDEPATH += $${LIBS_PATH}\hdf5\include
LIBS += -L$${LIBS_PATH}\hdf5\lib\\$${ARCH} -lhdf5 -lhdf5_hl -lhdf5_tools -lhdf5_cpp
# levelDb
INCLUDEPATH += $${LIBS_PATH}\LevelDB\include
CONFIG(debug, debug|release): LIBS += -L$${LIBS_PATH}\LevelDB\lib\\$${ARCH}\v120\Debug -lLevelDb
CONFIG(release, debug|release): LIBS += -L$${LIBS_PATH}\LevelDB\lib\\$${ARCH}\v120\Release -lLevelDb
# lmdb
INCLUDEPATH += $${LIBS_PATH}\lmdb\include
CONFIG(debug, debug|release): LIBS += -L$${LIBS_PATH}\lmdb\lib\\$${ARCH} -llmdbD
CONFIG(release, debug|release): LIBS += -L$${LIBS_PATH}\lmdb\lib\\$${ARCH} -llmdb
#openblas
INCLUDEPATH += $${LIBS_PATH}\openblas\include
LIBS += -L$${LIBS_PATH}\openblas\lib\\$${ARCH} -llibopenblas
# caffe
INCLUDEPATH += $${LIBS_PATH}\caffe\include
CONFIG(debug, debug|release): LIBS += -L$${LIBS_PATH}\caffe\lib\\$${ARCH} -llibcaffed
CONFIG(release, debug|release): LIBS += -L$${LIBS_PATH}\caffe\lib\\$${ARCH} -llibcaffe
#eigen, igl
INCLUDEPATH += $${LIBS_PATH}\eigen
INCLUDEPATH += $${LIBS_PATH}\libigl\include
}

#QMAKE_CXXFLAGS += -pg
#LIBS += -pg

w#message($${LIBS})
