#include "motionproposal.h"

MotionProposal::MotionProposal()
{
    DISOptflow_type = cv::optflow::DISOpticalFlow::PRESET_ULTRAFAST;
    optflower = cv::optflow::createOptFlow_DIS(DISOptflow_type);
    optflower->setUseSpatialPropagation(use_spatial_propagation);
}

void MotionProposal::getProposal(Mat& frame_gray, vector<vector<Point>>& contours)
{
    TickMeter time;
    time.start();
    optflowDIS(frame_gray, contours);
    _studentInRegion(contours);
    time.stop();

    if (showWindow){
        cout << "proposal time:" << time.getTimeMilli() << "ms" << endl;
        drawContours(frame_gray, contours, -1, Scalar(255,255,0), 1);
        cv::imshow("proposal", frame_gray);
    }
}

void MotionProposal::setStudentRegion(const vector<cv::Point2d>& points)
{
    assert(points.size() == 4);
    studentRegion = points;
}

void MotionProposal::_studentInRegion(vector<vector<Point>>& contours)
{
    vector<Point> centers;
    for( auto i : contours){
        Moments m = moments(i, false);
        centers.push_back(Point(m.m10/m.m00, m.m01/m.m00));
    }
    vector<Point> region;
    region.push_back(Point(studentRegion[0].x*dst_width/src_width, studentRegion[0].y*dst_height/src_height));
    region.push_back(Point(studentRegion[1].x*dst_width/src_width, studentRegion[1].y*dst_height/src_height));
    region.push_back(Point(studentRegion[2].x*dst_width/src_width, studentRegion[2].y*dst_height/src_height));
    region.push_back(Point(studentRegion[3].x*dst_width/src_width, studentRegion[3].y*dst_height/src_height));


    vector<vector<Point>> new_contours;
    for(size_t i = 0; i < centers.size(); i++){
        //if(pointPolygonTest(studentRegion, centers[i], false) > 0 ){
        if(pointPolygonTest(region, centers[i], false) > 0 ){
            new_contours.push_back(contours[i]);
        }
    }
    contours = new_contours;
}

void MotionProposal::optflowDIS(Mat& frame_gray, vector<vector<Point>>& contours)
{
    if (!prev_gray.empty()){
        // prev_gray , frame_gray => fy
        Mat flow;
        optflower->calc(prev_gray, frame_gray, flow);

        vector<Mat> flow_vec;
        flow_vec.resize(2);
        split(flow, flow_vec);

        Mat fy_up, fy_down;
        float flow_motion_threshold = 0.5;
        cv::threshold(flow_vec[1], fy_up, flow_motion_threshold, 255, cv::THRESH_BINARY);
        cv::threshold(flow_vec[1], fy_down, -1*flow_motion_threshold, 255, cv::THRESH_BINARY_INV);

        cv::bitwise_or(fy_up, fy_down, fy);
        fy.convertTo(fy, CV_8UC1);

        //flow_vec[1].convertTo(debug_flow, CV_8UC1);
        //debug_flow *= 50;

        // fy => contours
        assert(fy.type() == CV_8UC1);
        cv::findContours(fy, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        //cout << "before: " << contours.size() << endl;
        int minLength = 2;
        double minArea = 2;
        vector<vector<Point>> new_contours;
        //for (auto i = contours.begin(); i != contours.end(); ++i) {
        for (int i = 0; i < contours.size(); i++){
            // approximate a polygonal curve
            approxPolyDP(contours[i], contours[i], 3, true);

            // filter contours
            if(contours[i].size() > minLength && contourArea(contours[i]) > minArea) {
                new_contours.push_back(contours[i]);
            }
        }
        contours.resize(new_contours.size());
        contours = new_contours;
        //cout << "after: " << contours.size() << endl;
    }
    prev_gray = frame_gray.clone();

}

void MotionProposal::setSize(int dst_w, int dst_h, int src_w, int src_h)
{
    dst_width = dst_w;
    dst_height = dst_h;
    src_width = src_w;
    src_height = src_h;
}
