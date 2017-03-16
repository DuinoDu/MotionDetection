#include "../include/motionproposal.h"
#include <opencv2/core.hpp>
#include <list>

#include <iostream>

using namespace std;
using cv::Size;
using cv::makePtr;
using cv::Moments;
using cv::Scalar;
using cv::FileNodeIterator;

/**
 * MotionProposal
 */

void MotionProposal::setImageSize(const int dst_w, const int dst_h, const int src_w, const int src_h)
{
    dst_width = dst_w;
    dst_height = dst_h;
    src_width = src_w;
    src_height = src_h;
}
void MotionProposal::setRegion(const vector<Point>& points)
{
    assert(points.size() == 4);
    roiRegion = points;
}
void MotionProposal::setShowWindow(const bool _flag)
{
    cout << "set show window" << endl;
    showWindow = _flag;
}
void MotionProposal::read_basic(const cv::FileNode& fn)
{
    //int dst_width, dst_height, src_width, src_height;
    //vector<Point> roiRegion;
    cout << "read MotionProposal config" << endl;
    showWindow = (string)fn["showWindow"] == "true" ? true: false;
    min_width = (int)fn["min_width_proposal"];
    min_height = (int)fn["min_height_proposal"];
    max_width = (int)fn["max_width_proposal"];
    max_height = (int)fn["max_height_proposal"];
}
void MotionProposal::convert2Rect(const vector<vector<Point> >& contours, vector<Rect>& proposals)
{
    proposals.clear();
    if (contours.size() > 0){
        for (auto c : contours){
            Rect rect;
            rect = boundingRect(c);
            proposals.push_back(rect);
        }
    }
}
void MotionProposal::filter_roiRegion(vector<vector<Point> >& contours)
{
    if (roiRegion.size() == 0)
        return;

    vector<Point> centers;
    for( auto i : contours){
        Moments m = moments(i, false);
        centers.push_back(Point(m.m10/m.m00, m.m01/m.m00));
    }
    vector<Point> region;
    region.push_back(Point(roiRegion[0].x, roiRegion[0].y));
    region.push_back(Point(roiRegion[1].x, roiRegion[1].y));
    region.push_back(Point(roiRegion[3].x, roiRegion[3].y));
    region.push_back(Point(roiRegion[2].x, roiRegion[2].y));

    vector<vector<Point> > new_contours;
    for(size_t i = 0; i < centers.size(); i++){
        if(pointPolygonTest(region, centers[i], false) > 0 ){
            new_contours.push_back(contours[i]);
        }
    }
    contours = new_contours;
}
void MotionProposal::filter_size(vector<Rect>& proposals)
{
    if (proposals.size() == 0)
        return;
    vector<Rect> new_p;
    for(const auto rect : proposals){
        if (rect.width > min_width && rect.width < max_width &&
            rect.height > min_height && rect.height < max_height){
            new_p.push_back(rect);
        }
    }
    proposals = new_p;
}
void MotionProposal::filter_nested(vector<Rect>& proposals)
{
    if (proposals.size() == 0)
        return;
    vector<Rect> new_p;
    for(const auto rect : proposals){
        bool isNested = false;
        for (auto other_rect : proposals){
            if(other_rect.x == rect.x && other_rect.y == rect.y &&
               other_rect.width == rect.width && other_rect.height == other_rect.height)
                continue;
            else{
                if ( other_rect.contains(Point(rect.x, rect.y)) &&
                     other_rect.contains(Point(rect.x+rect.width, rect.y)) &&
                     other_rect.contains(Point(rect.x, rect.y+rect.height)) &&
                     other_rect.contains(Point(rect.x+rect.width, rect.y+rect.height))){
                    isNested = true;
                    break;
                }
            }
        }
        if(!isNested) new_p.push_back(rect);
    }
    proposals = new_p;
}

/**
 * Frame Difference
 */

class FrameDiff : public MotionProposal
{
public:
    void getProposal(InputArray input, vector<Rect>& proposals);
    virtual void read(const FileNode& fn);
private:
    list<Mat> prev_gray_s;
    Mat diff_frame;
    void nms(vector<Rect>& proposals, const double nms_threshold);
    double IOU(const Rect& r1, const Rect& r2);

    /* parameter */
    double absThreshold = 10;
    int duration = 6;
    int interval = 2;
    double nms_threshold = 0.1;
};
void FrameDiff::getProposal(InputArray _input, vector<Rect>& proposals)
{
    CV_Assert(!_input.empty());
    CV_Assert(_input.type() == CV_8UC1);
    Mat input = _input.getMat();
    vector<vector<Point> > contours;

    // store prev images
    prev_gray_s.push_back(input.clone());
    if(prev_gray_s.size() > duration)
        prev_gray_s.erase(prev_gray_s.begin());

    // compute frame diff
    if(prev_gray_s.size() == duration){
        for (auto prev = prev_gray_s.begin(); prev != prev_gray_s.end();){
			cv::absdiff(input, *prev, diff_frame);
			cv::threshold(diff_frame, diff_frame, absThreshold, 255, CV_THRESH_BINARY);
			Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, Size(5, 5));
			Mat closed;
			cv::morphologyEx(diff_frame, closed, cv::MORPH_CLOSE, kernel);
            cv::morphologyEx(closed, closed, cv::MORPH_OPEN, kernel);

            vector<vector<Point> > c;
            cv::findContours(closed, c, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            contours.insert(contours.end(), c.begin(), c.end());
			for (int i = 0; i < interval; i++) prev++;
        }
    }

    filter_roiRegion(contours);
    convert2Rect(contours, proposals);
    filter_size(proposals);
    filter_nested(proposals);
    nms(proposals, nms_threshold);

    if (showWindow){
        for (auto rect : proposals)
            rectangle(input, rect, Scalar(255, 255, 0), 1);
        cv::imshow("proposal", input);
    }
}
void FrameDiff::read(const cv::FileNode &fn)
{
    read_basic(fn);
    cout << "read FrameDiff config" << endl;
    absThreshold = (double)fn["absThreshold"];
    duration = (int)fn["duration"];
    interval = (int)fn["interval"];
    nms_threshold = (double)fn["nms_threshold"];
}
void FrameDiff::nms(vector<Rect>& proposals, const double nms_threshold)
{
    vector<int> scores;
    for(auto i : proposals) scores.push_back(i.area());

    vector<int> index;
    for(int i = 0; i < scores.size(); ++i){
        index.push_back(i);
    }

    sort(index.begin(), index.end(), [&](int a, int b){
        return scores[a] > scores[b];
    }); 

    vector<bool> del(scores.size(), false);
    for(size_t i = 0; i < index.size(); i++){
        if( !del[index[i]]){
            for(size_t j = i+1; j < index.size(); j++){
                if(IOU(proposals[index[i]], proposals[index[j]]) > nms_threshold){
                    del[index[j]] = true;
                }
            }
        }
    }

    vector<Rect> new_proposals;
    for(const auto i : index){
        if(!del[i]) new_proposals.push_back(proposals[i]);
    }
    proposals = new_proposals;
}
double FrameDiff::IOU(const Rect& r1, const Rect& r2)
{
    int x1 = max(r1.x, r2.x);
    int y1 = max(r1.y, r2.y);
    int x2 = min(r1.x+r1.width, r2.x+r2.width);
    int y2 = min(r1.y+r1.height, r2.y+r2.height);
    int w = max(0, (x2-x1+1));
    int h = max(0, (y2-y1+1));
    double inter = w * h;
    double o = inter / (r1.area() + r2.area() - inter);
    return (o >= 0) ? o : 0;
}


/**
 * Optical flow --- DIS
 */

class OptflowDIS : public MotionProposal
{};

Ptr<MotionProposal> createMotionProposal(int algorithm)
{
    switch (algorithm) {
        case NO:
            return makePtr<MotionProposal>();
        case FRAMEDIFF:
            return makePtr<FrameDiff>();
        case DIS:
            CV_Assert(false); 
            return makePtr<MotionProposal>();
        default:
            CV_Assert(false);
            return makePtr<MotionProposal>();
    }
}
