#ifndef OBJECTTRACK_HPP
#define OBJECTTRACK_HPP

#include <opencv2/opencv.hpp>
#include <vector>

using std::vector;

using cv::Mat;
using cv::Rect;
using cv::Rect2d;
using cv::Algorithm;
using cv::Ptr;
using cv::Point;
using cv::FileNode;
using cv::makePtr;

#include <iostream>
using std::cout;
using std::endl;

class ObjectTrack : public Algorithm
{
public:
    virtual void read(const FileNode& fn) = 0;
    virtual void update(const Mat& img, const vector<vector<int>>& boxes) = 0;
    int getObjectNum(){ return objects.size(); }
    vector<Rect2d> getBoxes(){ return objects; }
    vector<double> getMovements(){ return movements; }
    vector<int> getTimes(){ return times; }

protected:
    void read_basic(const FileNode& fn);
    vector<Rect2d> objects;		// current tracking objects
    vector<double> movements;	// move distance of each object, starting from firstly detected, only at Y
    vector<int> times;			// frame number of being still for each object

    /* parameters */
    int maxObjects = 10;
};


Ptr<ObjectTrack> createObjectTrack();

#endif
