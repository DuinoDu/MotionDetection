#include "motiondetection.h"

MotionDetection::MotionDetection()
{
    proposal = new MotionProposal;
    classification = new MotionClassification;
}

MotionDetection::~MotionDetection()
{
    delete proposal;
    delete classification;
}

void MotionDetection::setParam(string paramName, float value)
{
    if (paramName == "RoomWidth"){}
    if (paramName == "RoomHeight"){}
}
void MotionDetection::setParam(string paramName, string value)
{
    if (paramName == "CaffemodelPath")
            classification->setCaffemodelPath(value);
}
void MotionDetection::setParam(string paramName, vector<int> value)
{
    if(paramName == "StudentRegion"){
            assert(value.size() == 8);
            vector<Point2d> region;
            for(int i = 0; i < 4; ++i){
                double x = value[i];
                double y = value[2*i+1];
                region.push_back(Point2d(x, y));
            }
            proposal->setStudentRegion(region);
    }
}
void MotionDetection::showWindows(bool flag1, bool flag2)
{
    proposal->showWindow = flag1;
    classification->showWindow = flag2;
}

void MotionDetection::detect()
{
    result.clear();
    if (frame.isContinuous() && !frame.empty()){
        proposal->setSize(dst_width, dst_height, frame.cols, frame.rows);
        resize(frame, frame, Size(dst_width, dst_height));
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

        proposal->getProposal(frame_gray, contours);
        classification->classify(frame, contours, result);

        //result.push_back({100, 100});
        //result.push_back({200, 300});
        //result.push_back({300, 400});
        returnResult(result, context);
    }
}

void MotionDetection::setCallback(MyFun _myFun, void *_context)
{
    context  = _context;
    returnResult  =_myFun;
}

void MotionDetection::setFrame(Mat& _frame)
{
    frame = _frame;
}

void MotionDetection::setFrame(uchar* data, int height, int width, int length)
{
    assert(length/(height*width) == 3);
    frame = Mat(height, width, CV_8UC3, (void*)data);
}
