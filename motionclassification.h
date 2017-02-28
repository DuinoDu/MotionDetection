#ifndef MOTIONCLASSIFICATION_H
#define MOTIONCLASSIFICATION_H

#include "common.h"
#include "objectdetection.h"
#include "objecttrack.h"
#include "actionrecognition.h"

class MotionClassification
{
public:
    MotionClassification();
    ~MotionClassification();
    void classify(Mat& frame, vector<vector<Point>>contours, vector<vector<int>> &result);
    void setCaffemodelPath(string path);
    bool showWindow = false;

private:
    ObjectDetection *faceDetection;
    ObjectTrack *faceTrack;
    ActionRecognition *recognition;
    vector<vector<int>> faceBoxes;
};

#endif // MOTIONCLASSIFICATION_H
