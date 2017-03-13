######################################################################
# Automatically generated by qmake (3.0) ?? 3? 6 15:53:43 2017
######################################################################

TEMPLATE = app
TARGET = test
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH += .

# Input
HEADERS += interface.h
SOURCES += main.cpp

win32{
LIBS_PATH = E:\debug\caffe_environment_x86_x64
ARCH = x86
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
}
CONFIG(release, debug|release){
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_core$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_highgui$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgcodecs$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_imgproc$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_video$${VERSION_OPENCV}
LIBS += -L$${PATH_OPENCV_LIBRARIES} -lopencv_videoio$${VERSION_OPENCV}
}
}
