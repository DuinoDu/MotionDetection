#ifndef MOTIONPROPOSAL_H
#define MOTIONPROPOSAL_H

#include "common.h"
#include <opencv2/optflow.hpp>

class MotionProposal
{
public:
    MotionProposal();
    void getProposal(Mat& frame_gray, vector<vector<Point>>& contours);
    void setSize(int dst_w, int dst_h, int src_w, int src_h);
    void setStudentRegion(const vector<Point2d>& points);
    bool showWindow = false;

private:
    // dense optical flow DIS
    void optflowDIS(Mat& frame_gray, vector<vector<Point>>& contours);
    bool use_spatial_propagation = true; // fixed number of stripes
    bool use_temporal_propagation = true;
    int DISOptflow_type;
    Ptr<cv::optflow::DISOpticalFlow> optflower;
    Mat fy;

    Mat prev_gray;
    int dst_width;
    int dst_height;
    int src_width;
    int src_height;
    void _studentInRegion(vector<vector<Point>>& contours);
    vector<Point2d> studentRegion;

};

#endif // MOTIONPROPOSAL_H
