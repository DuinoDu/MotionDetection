#ifndef FACETRACKER_H
#define FACETRACKER_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

#include "common.h"
using cv::MultiTracker;

class ObjectTrack
{
public:
    ObjectTrack();
    ~ObjectTrack();
    void setType(const string& type);
    void update(const Mat& img, const vector<vector<int>>& boxes);
    int getObjectNum();
    vector<Rect2d> getBoxes();
    vector<double> getMovements();

    void add_test(const Mat& img, const vector<int>& box);
    void update_test(const Mat& img);
    Rect2d getCurrentBox_test();

private:
    MultiTracker* multiTracker;
    vector<Rect2d> objects;
    vector<double> movements;
    vector<Point2d> origins;
    vector<int> times;
    int stillImageTime = 20;
    int stillDisThreshold = 10;

    /*private func*/
    void _addBox(const Mat& img, const vector<int>& new_box);
    void _updateMovements();
    void _updateTimes();
    void _updateObjects();
    void _removeStillBox();

    bool _isOldBox(vector<int> box);
};

#endif
