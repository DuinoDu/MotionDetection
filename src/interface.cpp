#include "../include/interface.h"
#include "../include/motiondetection.h"
#include "../include/headlayer.h"
#include "../include/log.h"

extern "C"
{

__declspec(dllexport) void * createMotionDetector()
{
    MotionDetection *detector;
    detector = new MotionDetection;
    return detector;
}

__declspec(dllexport) void deleteMotionDetector(void * detector)
{
    delete detector;
}

__declspec(dllexport) void setRoomSize(void* _detector,  int width, int height)
{
    MotionDetection * detector = (MotionDetection*)_detector;
    detector->setParam("RoomWidth", width);
    detector->setParam("RoomHeight", height);
}

__declspec(dllexport) void setMotionSize(void* _detector, float standup, float sitdown)
{
	MotionDetection * detector = (MotionDetection*)_detector;
	detector->setParam("standup_distance", standup);
	detector->setParam("sitdown_distance", sitdown);
}

__declspec(dllexport) void setStudentRegion(void* _detector,  int topleft_x, int topleft_y, int topright_x, int topright_y,
                      int bottomleft_x, int bottomleft_y, int bottomright_x, int bottomright_y)
{
    //vector<int> studentRegion = {764, 174, 1510, 206, 1508, 554, 2, 346}; // x1, y1,...x4, y4
    std::vector<int> studentRegion;
    studentRegion.push_back(topleft_x);
    studentRegion.push_back(topleft_y);
    studentRegion.push_back(topright_x);
    studentRegion.push_back(topright_y);
    studentRegion.push_back(bottomleft_x);
    studentRegion.push_back(bottomleft_y);
    studentRegion.push_back(bottomright_x);
    studentRegion.push_back(bottomright_y);
    MotionDetection * detector = (MotionDetection*)_detector;
    detector->setParam("StudentRegion", studentRegion);
}

__declspec(dllexport) void setCaffemodelPath(void* _detector, char* str)
{
    MotionDetection * detector = (MotionDetection*)_detector;
    std::string path(str);  //"D:/code/mtcnn/mtcnn/model"
    detector->setParam("CaffemodelPath", path);
}

__declspec(dllexport) void setCallback(void* _detector,  MyFun functionName, void* context)
{
    MotionDetection * detector = (MotionDetection*)_detector;
    detector->setCallback(functionName, context);
}

__declspec(dllexport) void setShowWindows(void* _detector,  bool stage1, bool stage2)
{
    MotionDetection * detector = (MotionDetection*)_detector;
    detector->showWindows(stage1, stage2);
}

__declspec(dllexport) void setFrame(void* _detector, unsigned char* data, int height, int width, int length)
{
    MotionDetection * detector = (MotionDetection*)_detector;
    detector->setFrame(data, height, width, length);
}

}
