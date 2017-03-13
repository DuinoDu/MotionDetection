#ifndef MOTIONPROPOSAL_H
#define MOTIONPROPOSAL_H

#include "common.h"
#include <opencv2/optflow.hpp>

class MotionProposal
{
public:
    MotionProposal();
	//void getProposal(Mat& frame_gray, vector<vector<Point>>& contours);
	void getProposal(Mat& frame_gray, vector<Rect>& proposals);
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

    list<Mat> prev_gray_s;
	Mat prev_gray;
    int dst_width;
    int dst_height;
    int src_width;
    int src_height;
    void _studentInRegion(vector<vector<Point>>& contours);
    vector<Point2d> studentRegion;

	void bgModel(Mat& frame_gray, vector<vector<Point>>& contours);

	void frameSub(Mat& frame_gray, vector<vector<Point>>& contours);
	Mat diff_frame;
	void _filterProposals(vector<Rect>& proposals);
	void _nms(vector<Rect>& proposals, double nms_threshold);
	double IOU(const Rect& r1, const Rect& r2);

	// parameter for contours
	float flow_motion_threshold = 0.5; 
	double maxArea = 50000;
	double minArea = 20;
	int minLength = 10;
	
	// parameter for absdiff
	double absThreshold = 10;
	int duration = 6;
	int interval = 2;
	double nms_threshold = 0.1;

};

#endif // MOTIONPROPOSAL_H
