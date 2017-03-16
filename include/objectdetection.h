#ifndef OBJECTDETECTION_HPP
#define OBJECTDETECTION_HPP

#include <opencv2/opencv.hpp>
#include <vector>

using std::vector;
using std::string;
using cv::Mat;
using cv::Rect;
using cv::Algorithm;
using cv::FileNode;
using cv::Ptr;
using cv::makePtr;

class ObjectDetection : public Algorithm
{
public:
    virtual void read(const FileNode& fn) = 0;
    virtual void detect(const Mat& input, vector<Rect>& proposals, vector<vector<int> >& object_box) = 0;
    virtual void setCaffeModelPath(const string& path) = 0;

protected:
    void read_basic(const FileNode& fn);
    void filter(vector<vector<int> >& object_box);
};


Ptr<ObjectDetection> createObjectDetection();

#endif
