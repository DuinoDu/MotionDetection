#ifndef MOTIONDETECTION_H
#define MOTIONDETECTION_H


#include "common.h"
#include "motionproposal.h"
#include "motionclassification.h"

#ifndef WIN32
#define __stdcall
#endif

typedef void (__stdcall* MyFun)(vector<vector<int> > result, void* context);

class MotionDetection
{
public:
    MotionDetection();
    ~MotionDetection();

    void setParam(string paramName, float value);
    void setParam(string paramName, string value);
    void setParam(string paramName, vector<int> value);
    void showWindows(bool flag1, bool flag2);

    void detect();
    void setCallback(MyFun _myFun, void* _context);
    void setFrame(Mat& _frame);
    void setFrame(uchar* data, int height, int width, int length);

private:
    Mat frame;
    Mat frame_gray;
    MyFun returnResult;
    void* context;

    void _updateParams();

    MotionProposal *proposal;
    MotionClassification *classification;
    vector<vector<Point>> contours;
    vector<vector<int>>  result;

    int dst_width = 720;
    int dst_height = 416;
};

#endif // MOTIONDETECTION_H
