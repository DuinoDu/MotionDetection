#ifndef ACTIONRECOGNITION_HPP
#define ACTIONRECOGNITION_HPP

#include "objectdetection.h"
#include "objecttrack.h"

using cv::Rect2d;

class ActionRecognition
{
};

class StandupRecognition : public ActionRecognition
{
public:
    void read(const FileNode& fn);
    void classify(const Mat& input);
    void setObjectDetector(ObjectDetection* _p){ if(_p!=nullptr) detector = _p; }
    void setObjectTracker(ObjectTrack* _p){ if(_p!=nullptr) tracker = _p; }
    vector<Rect> getObjects(){ return objects; }

private:
    ObjectDetection* detector;
    ObjectTrack* tracker;
    vector<Rect> objects;
    int frame_cnt = 0;

    void checkObjectExist(const Mat& input);
    int getStandupDisTh(const Rect2d& box);
    int getStandupROI_index(const Rect2d& box);

    /* parameters */
    double x_paddingscale_standup = 0.5;
    double standup_threshold_scale = 1;
    double x_paddingscale_while_standing = 0.5;
    double y_paddingscale_while_standing = 0.3;
    int check_frames = 8;  // check if face exists in region
    int max_standup = 2;
};

Ptr<StandupRecognition> createStandupRecognition();

#endif
