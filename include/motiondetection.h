#ifndef MOTIONDETECTION_H
#define MOTIONDETECTION_H

#include "common.h"
#include "motionproposal.h"
#include "objectdetection.h"
#include "objecttrack.h"
#include "actionrecognition.h"

//typedef void(_stdcall *MyFun)(vector<vector<int>>& result, void* context);
typedef void(_stdcall *MyFun)(int* px, int* py, size_t size, void* context);

class MotionDetection
{
public:
    MotionDetection();

    void setParam(string paramName, float value);
    void setParam(string paramName, string value);
    void setParam(string paramName, vector<int> &value);
    void showWindows(bool flag1, bool flag2);

    void setCallback(MyFun _myFun, void* _context);
    void setFrame(Mat& _frame);
    void setFrame(uchar* data, int height, int width, int length);

private:
    MyFun returnResult;
    void* context;
    bool showWindow = false;

	void readConfig(const string& configPath);
    void detect(Mat& frame);
    void _updateParams();
    void _sendResult(const vector<vector<int>>& result);

    Ptr<MotionProposal> proposal;
    Ptr<ObjectDetection> faceDetector;
    Ptr<ObjectTrack> faceTracker;
    Ptr<StandupRecognition> recognition;

    bool _isTrackingObject(const Rect2d& rect, const vector<Rect2d>& objects);
	
	// parameter
	double minProposal_w = 20;
	double minProposal_h = 40;
	double maxProposal_w = 100;
	double maxProposal_h = 300;

};

#endif // MOTIONDETECTION_H
