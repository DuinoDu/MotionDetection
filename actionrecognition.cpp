#include "actionrecognition.h"

ActionRecognition::ActionRecognition(){}

void ActionRecognition::detect(ObjectTrack *tracker)
{
    // check tracker's movements. If movements[i] exceed threshold, then generate a standup region.
    vector<double> movements = tracker->getMovements();
    vector<Rect2d> boxes = tracker->getBoxes();
    assert(movements.size() == boxes.size());

    for(int i = 0; i < movements.size(); ++i){
        // detect standup
        if(movements[i] < _getStandupDisTh(boxes[i])){
            // <0
            int index = _isBoxInStanupROI(boxes[i]);
            if(index > -1){
                detectROI.faceROI[index] = boxes[i];
                detectROI.standupROI[index] = _padding(boxes[i]);
            }
            else{
                detectROI.faceROI.push_back(boxes[i]);
                detectROI.standupROI.push_back(_padding(boxes[i]));
            }
        }

        // detect sitdown
        // If KCF tracker is enough fast, it can keep tracking face until he sits down.
        // Keeping tracking can handle more situations.
        // It remains a problem.
        if(movements[i] > _getSitdownDisTh(boxes[i])){
            // >0
            int index = _getStandupROI_index(boxes[i]);
            assert((index >= -1) && (index < (int)detectROI.faceROI.size()));
            if(index > -1){
                detectROI.faceROI.erase(detectROI.faceROI.begin() + i);
                detectROI.standupROI.erase(detectROI.standupROI.begin() + i);
            }
        }
    }
}

int ActionRecognition::_getStandupDisTh(Rect2d box)
{
    return -15;
}

int ActionRecognition::_getSitdownDisTh(Rect2d box)
{
    return 10;
}

int ActionRecognition::_isBoxInStanupROI(Rect2d box, double threshold)
{
    int minDis = 65535;
    int index = -1;
    for(int i = 0; i< detectROI.standupROI.size(); ++i){
        double dis = _disBtwTwoBox(box, detectROI.standupROI[i], 1);
        if(dis < minDis){
            minDis = dis;
            index = i;
        }
    }
    if(minDis < threshold)
        return index;
    else
        return -1;
}

int ActionRecognition::_isBoxInSitdownROI(Rect2d box, double threshold)
{
    int minDis = 65535;
    int index = -1;
    for(int i = 0; i< detectROI.sitdownROI.size(); ++i){
        double dis = _disBtwTwoBox(box, detectROI.sitdownROI[i], 1);
        if(dis < minDis){
            minDis = dis;
            index = i;
        }
    }
    if(minDis < threshold)
        return index;
    else
        return -1;
}

double ActionRecognition::_disBtwTwoBox(Rect2d box1, Rect2d box2, int flag)
{
    double distance = 0.0;
    double center1_x = box1.x + box1.width/2;
    double center1_y = box1.y + box1.height/2;
    double center2_x = box2.x + box2.width/2;
    double center2_y = box2.y + box2.height/2;
    if (flag == 0){
        distance = std::abs(center1_y - center2_y);
    }
    else if(flag == 1){
        distance = std::abs(center1_x - center2_x) + std::abs(center1_y - center2_y);
    }
    return distance;
}

Rect2d ActionRecognition::_padding(Rect2d& src)
{
    Rect2d dst;
    dst.x = std::max(0.0, src.x - detectROI.padding_w);
    dst.y = std::max(0.0, src.y - detectROI.padding_h);
    dst.width = src.width + detectROI.padding_w*2;
    dst.height = src.height + detectROI.padding_h*2;
    return dst;
}

int ActionRecognition::_getStandupROI_index(Rect2d& box, int type)
{
    int index = -1;
    Point2d center;
    center.x = box.x + box.width/2;
    center.y = box.y + box.height/2;
    int neighbor = 10;
    vector<Point2d> p1s, p2s;
    for(auto roi : detectROI.standupROI){
        Point2d p1(roi.x, roi.y);
        Point2d p2(roi.x + roi.width, roi.y + roi.height);
        p1s.push_back(p1);
        p2s.push_back(p2);
    }

    // If there is only one people standing up, then he should sitdown.
    if(detectROI.standupROI.size() == 1){
        return 0;
    }

    switch (type) {
    case 0:  // box center in ROI
        for(int i = 0; i < detectROI.standupROI.size(); i++){
            if(detectROI.standupROI[i].contains(center)){
                index = i;
                break;
            }
        }
        break;
    case 1:  // box x in ROI, y in neighbor
        for(int i = 0; i < detectROI.standupROI.size(); i++){
            if((p1s[i].x < center.x) && (p2s[i].x > center.x)){
                if((p1s[i].y-neighbor < center.y) && (p2s[i].y + neighbor > center.y)){
                    index = i;
                    break;
                }
            }
        }
        break;
    case 2:  // box y in ROI, x in neighbor
        for(int i = 0; i < detectROI.standupROI.size(); i++){
            if((p1s[i].y < center.y) && (p2s[i].y > center.y)){
                if((p1s[i].x-neighbor < center.x) && (p2s[i].x + neighbor > center.x)){
                    index = i;
                    break;
                }
            }
        }
        break;
    case 3:  // box x and y in neighbor
        for(int i = 0; i < detectROI.standupROI.size(); i++){
            if((p1s[i].y-neighbor < center.y) && (p2s[i].y + neighbor > center.y)){
                if((p1s[i].x-neighbor < center.x) && (p2s[i].x + neighbor > center.x)){
                    index = i;
                    break;
                }
            }
        }
        break;
    default:
        break;
    }
    return index;
}
