#include "../include/motionproposal.h"
#include "../include/log.h"

MotionProposal::MotionProposal()
{
    DISOptflow_type = cv::optflow::DISOpticalFlow::PRESET_ULTRAFAST;
    optflower = cv::optflow::createOptFlow_DIS(DISOptflow_type);
    optflower->setUseSpatialPropagation(use_spatial_propagation);
}

//void MotionProposal::getProposal(Mat& frame_gray, vector<vector<Point>>& contours)
void MotionProposal::getProposal(Mat& frame_gray, vector<Rect>& proposals)
{
	proposals.clear(); 
	vector<vector<Point>> contours;

	frameSub(frame_gray, contours);
    _studentInRegion(contours);

	// convert vector<vector<Point>> to vector<Rect>
	if (contours.size() > 0){
		for (auto c : contours){
			Rect rect;
			rect = boundingRect(c);
			proposals.push_back(rect);
		}
	}

	// filter proposals, using nms,....
	_filterProposals(proposals);

	/*
    if (showWindow){
		cv::line(frame_gray, studentRegion[0], studentRegion[1], cv::Scalar(0,0,255), 2);
		cv::line(frame_gray, studentRegion[1], studentRegion[3], cv::Scalar(0, 0, 255), 2);
		cv::line(frame_gray, studentRegion[3], studentRegion[2], cv::Scalar(0, 0, 255), 2);
		cv::line(frame_gray, studentRegion[2], studentRegion[0], cv::Scalar(0, 0, 255), 2);
        drawContours(frame_gray, contours, -1, Scalar(255,255,0), 1);
        cv::imshow("proposal", frame_gray);
    }
	*/
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

        vector<vector<Point>> new_contours;
        //for (auto i = contours.begin(); i != contours.end(); ++i) {
        for (size_t i = 0; i < contours.size(); i++){
            // approximate a polygonal curve
            approxPolyDP(contours[i], contours[i], 3, true);

            // filter contours
			if (contours[i].size() > minLength && contourArea(contours[i]) > minArea && contourArea(contours[i]) < maxArea) {
                new_contours.push_back(contours[i]);
            }
        }
        contours.resize(new_contours.size());
        contours = new_contours;
        //cout << "after: " << contours.size() << endl;
    }
    prev_gray = frame_gray.clone();
}

void MotionProposal::frameSub(Mat& frame_gray, vector<vector<Point>>& contours)
{
	prev_gray_s.push_back(frame_gray.clone());
	if (prev_gray_s.size() > duration)
		prev_gray_s.erase(prev_gray_s.begin());

	if (prev_gray_s.size() == duration){
		for (auto prev = prev_gray_s.begin(); prev != prev_gray_s.end(); ){
			vector<vector<Point>> c;
			cv::absdiff(frame_gray, *prev, diff_frame);
			cv::threshold(diff_frame, diff_frame, absThreshold, 255, CV_THRESH_BINARY);
			Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, Size(5, 5));
			Mat closed;
			cv::morphologyEx(diff_frame, closed, cv::MORPH_CLOSE, kernel);
			cv::morphologyEx(closed, closed, cv::MORPH_OPEN, kernel);
			cv::findContours(closed, c, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
			contours.insert(contours.end(), c.begin(), c.end());

			for (int i = 0; i < interval; i++) prev++;
		}
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
    region.push_back(Point(studentRegion[0].x, studentRegion[0].y));
    region.push_back(Point(studentRegion[1].x, studentRegion[1].y));
    region.push_back(Point(studentRegion[3].x, studentRegion[3].y));
    region.push_back(Point(studentRegion[2].x, studentRegion[2].y));

    vector<vector<Point>> new_contours;
    for(size_t i = 0; i < centers.size(); i++){
        if(pointPolygonTest(region, centers[i], false) > 0 ){
            new_contours.push_back(contours[i]);
        }
    }
    contours = new_contours;
}

void MotionProposal::setSize(int dst_w, int dst_h, int src_w, int src_h)
{
    dst_width = dst_w;
    dst_height = dst_h;
    src_width = src_w;
    src_height = src_h;
}

void MotionProposal::_filterProposals(vector<Rect>& proposals)
{
	_nms(proposals, nms_threshold);
}
void MotionProposal::_nms(vector<Rect>& proposals, double nms_threshold)
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
double MotionProposal::IOU(const Rect& r1, const Rect& r2)
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