#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <opencv2/opencv.hpp>


#ifdef WIN32
#include "headlayer.h"
#endif

#ifndef WIN32
#define __stdcall
#endif

#include "motiondetection.h"

using namespace std;
using cv::Mat;

using cv::VideoCapture;

void __stdcall getResult(vector<vector<int> >result, void* context)
{
    for(auto point : result){
        assert(point.size() == 2);
        cout << "x:" << point[0] << ", y:" << point[1] << endl;
    }
}

void initDetector(MotionDetection& detector, const string& modelPath)
{
    detector.setParam("RoomWidth", 800.0);
    detector.setParam("RoomHeight", 600.0);
    vector<int> studentRegion = {764, 174, 1510, 206, 1508, 554, 2, 346}; // x1, y1,...x4, y4
    detector.setParam("StudentRegion", studentRegion);

    //detector.setParam("CaffemodelPath", "/home/duino/project/iactive/mtcnn/mtcnn/model/");
    detector.setParam("CaffemodelPath", modelPath);
    //detector.setParam("CaffemodelPath", "D:/code/mtcnn/mtcnn/model");
    detector.setCallback((MyFun) getResult, NULL);
    detector.showWindows(false, false);
}

int main(int argc, char *argv[])
{
    //string videoPath = "H:/iactive/windows_iactive/iactive/program/video/0.mp4";
    //string videoPath = "D:/video/0.mp4";
    if (argc != 3){
        cout << "Usage: ./MotionDetection [model_path] [video_path]" << endl;
        return -1;
    }
    string videoPath = argv[1];  //"/home/duino/Videos/3/0.mp4";
    string modelPath = argv[2];
    VideoCapture cam;
    Mat frame;

    MotionDetection detector;
    initDetector(detector, modelPath);

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
            cv::waitKey(20);
            cam >> frame;
        }
    }
    cout << "Finish! " << endl;
    return 0;
}

