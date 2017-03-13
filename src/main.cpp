#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <opencv2/opencv.hpp>


#ifdef _WIN32
#include "headlayer.h"
#endif

#include "motiondetection.h"

using namespace std;
using cv::Mat;

using cv::VideoCapture;

void _stdcall getResult(vector<vector<int>>result)
{
    for(auto point : result){
        assert(point.size() == 2);
        cout << "x:" << point[0] << ", y:" << point[1] << endl;
    }
}

void initDetector(MotionDetection& detector)
{
    detector.setParam("RoomWidth", 800.0);
    detector.setParam("RoomHeight", 600.0);
    vector<int> studentRegion = {764, 174, 1510, 206, 1508, 554, 2, 346}; // x1, y1,...x4, y4
    detector.setParam("StudentRegion", studentRegion);

    //detector.setParam("CaffemodelPath", "/home/duino/project/iactive/mtcnn/mtcnn/model/");
    detector.setParam("CaffemodelPath", "D:/code/mtcnn/mtcnn/model");
    detector.setCallback((MyFun) getResult, NULL);
    detector.showWindows(false, true);
}

int main(int argc, char *argv[])
{
    //string videoPath = "H:/iactive/windows_iactive/iactive/program/video/0.mp4";
    string videoPath = "D:/video/0.mp4";
    VideoCapture cam;
    Mat frame;

    MotionDetection detector;
    initDetector(detector);

    if (cam.open(videoPath)){
        cam >> frame;
        while (frame.isContinuous() && !frame.empty()){

            uchar *data = frame.data;
            int height = frame.rows;
            int width = frame.cols;
            int length = height*width*frame.channels();

            detector.setFrame(data, height, width, length);
            //detector.setFrame(frame);

            detector.detect();

            //cv::imshow("video", frame);
            cv::waitKey(1000);
            cam >> frame;
        }
    }
    cout << "Finish! " << endl;
    return 0;
}

