#include "../include/actionrecognition.h"

void StandupRecognition::read(const FileNode& fn)
{
    cout << "read StandupRecognition config" << endl;
    x_paddingscale_standup = (double)fn["x_paddingscale_standup"];
    standup_threshold_scale = (double)fn["standup_threshold_scale"];
    x_paddingscale_while_standing = (double)fn["x_paddingscale_while_standing"];
    y_paddingscale_while_standing = (double)fn["y_paddingscale_while_standing"];
    check_frames = (int)fn["check_frames"];
    max_standup = (int)fn["max_standup"];
}

void StandupRecognition::classify(const Mat& input)
{
    if(tracker == nullptr) return;
    vector<Rect2d> boxes = tracker->getBoxes();
    vector<double> movements = tracker->getMovements();
    CV_Assert(boxes.size() == movements.size());

    // detect standup
    for(size_t i = 0; i < boxes.size(); ++i){
        if(movements[i] < getStandupDisTh(boxes[i])){
            // < 0
            int index = getStandupROI_index(boxes[i]);
            if(index > -1){
                objects[index] = boxes[i]; // update
            }
            else if(index == -1 && objects.size() <= max_standup){
                objects.push_back(boxes[i]); // add
            }
        }
    }
    checkObjectExist(input);
}

void StandupRecognition::checkObjectExist(const Mat& input)
{
    if (frame_cnt > check_frames && detector != nullptr){
        for (size_t i = 0; i < objects.size(); i++){
            vector<Rect> proposals;
            vector<vector<int>> result;
            Rect roi;
            roi.x = std::max(0.0, objects[i].x - x_paddingscale_while_standing * objects[i].width);
            roi.width = objects[i].width * (1 + x_paddingscale_while_standing * 2);
            roi.y = std::max(0.0, objects[i].y - y_paddingscale_while_standing * objects[i].height);
            roi.height = objects[i].height * (1 + y_paddingscale_while_standing * 2);
            proposals.push_back(roi);

            detector->detect(input, proposals, result);
            if (result.empty()){
                objects.erase(objects.begin() + i);
            }
        }
        frame_cnt = 0;
    }
    frame_cnt++;
}

int StandupRecognition::getStandupDisTh(const Rect2d& box)
{
    return -1 * standup_threshold_scale * box.height;
}

int StandupRecognition::getStandupROI_index(const Rect2d& box)
{
    int index = -1;
    double center_x = box.x + box.width / 2;
    for (size_t i = 0; i < objects.size(); ++i){
        double x1 = objects[i].x - objects[i].width * x_paddingscale_standup;
        double x2 = objects[i].x + objects[i].width * (1 + x_paddingscale_standup);
        if (center_x > x1 && center_x < x2){
            index = i;
            break;
        }
    }
    return index;
}

Ptr<StandupRecognition> createStandupRecognition()
{
    return makePtr<StandupRecognition>();
}
