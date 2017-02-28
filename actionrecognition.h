#ifndef ACTIONRECOGNITION_H
#define ACTIONRECOGNITION_H

#include "common.h"
#include "objecttrack.h"


struct DetectROI{
    vector<Rect2d> faceROI;
    vector<Rect2d> standupROI;
    vector<Rect2d> sitdownROI;
    int padding_w = 8;
    int padding_h = 12;
};

class ActionRecognition
{
public:
    ActionRecognition();
    void detect(ObjectTrack *tracker);
    DetectROI getDetectROI(){return detectROI;}

private:
    DetectROI detectROI;

    int minTime = 5;
    int minFaceDistance = 10;
    int minStandupDis = 20;

    int _getStandupDisTh(Rect2d box);
    int _getSitdownDisTh(Rect2d box);
    int _isBoxInStanupROI(Rect2d box, double threshold = 20); // influenced by fps
    int _isBoxInSitdownROI(Rect2d box, double threshold = 20);
    double _disBtwTwoBox(Rect2d box1, Rect2d box2, int flag = 1);
    Rect2d _padding(Rect2d& src);
    int _getStandupROI_index(Rect2d& box, int type = 0);
};

#endif // ACTIONRECOGNITION_H
