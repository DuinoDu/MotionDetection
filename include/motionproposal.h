#ifndef MOTIONPROPOSAL_HPP
#define MOTIONPROPOSAL_HPP

#include <opencv2/opencv.hpp>
#include <vector>

using std::vector;

using cv::Mat;
using cv::Rect;
using cv::Algorithm;
using cv::InputArray;
using cv::Ptr;
using cv::Point;
using cv::FileNode;
using cv::makePtr;

#include <iostream>
using std::cout;
using std::endl;

class MotionProposal : public Algorithm
{
public:
    void setImageSize(const int dst_w, const int dst_h, const int src_w, const int src_h);
    void setRegion(const vector<Point>& points);
    void setShowWindow(const bool);
    virtual void getProposal(InputArray input, vector<Rect>& proposals){ cout << "Please set proposal method" << endl;}
    virtual void read(const FileNode& fn){ read_basic(fn);}

protected:
    void read_basic(const FileNode& fn);
    void convert2Rect(const vector<vector<Point> >& contours, vector<Rect>& proposals);
    void filter_roiRegion(vector<vector<Point> >& contours);
    void filter_size(vector<Rect>& proposals);
    void filter_nested(vector<Rect>& proposals);

    /* parameter */
    int dst_width, dst_height, src_width, src_height;
    vector<Point> roiRegion;
    bool showWindow;
    int min_width, min_height, max_width, max_height;
};

enum PROPOSAL { FRAMEDIFF = 101, DIS = 102, NO = 100};

Ptr<MotionProposal> createMotionProposal(int algorithm);

#endif
