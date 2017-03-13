#include "../include/actionrecognition.h"

ActionRecognition::ActionRecognition(){}

void ActionRecognition::detect(const Mat& frame, ObjectTrack *tracker, FaceDetector* faceDetector)
{
    // check tracker's movements. If movements[i] exceed threshold, then generate a standup region.
	if (tracker == nullptr){
		return;
	}
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
            }
			else if(index == -1 && detectROI.faceROI.size() <= max_standup){
                detectROI.faceROI.push_back(boxes[i]);
            }
        }
        // detect sitdown
		/*
		else if(movements[i] > _getSitdownDisTh(boxes[i])){
            // >0
            int index = _getStandupROI_index(boxes[i]);
            assert((index >= -1) && (index < (int)detectROI.faceROI.size()));
            if(index > -1){
                detectROI.faceROI.erase(detectROI.faceROI.begin() + i);
            }
        }
		*/
    }
	
	// check if detectROI contains face
	if (frame_cnt > check_period && faceDetector != nullptr){
		for (size_t i = 0; i < detectROI.faceROI.size(); i++){
			Mat roi = frame.rowRange(detectROI.faceROI[i].y - padding_while_standing_y, detectROI.faceROI[i].y + detectROI.faceROI[i].height + padding_while_standing_y)
				.colRange(detectROI.faceROI[i].x - padding_while_standing_x, detectROI.faceROI[i].x + detectROI.faceROI[i].width + padding_while_standing_x);
			vector<vector<int>> detect_result;
			faceDetector->detect(roi, detect_result);
			if (detect_result.empty()){
				detectROI.faceROI.erase(detectROI.faceROI.begin() + i);
			}
		}
		frame_cnt = 0;
	}
	frame_cnt++;
	
}

int ActionRecognition::_getStandupDisTh(Rect2d box)
{
    return -1 * standup_t * box.height;
}

int ActionRecognition::_getSitdownDisTh(Rect2d box)
{
	return sitdown_t * box.height;
}

int ActionRecognition::_isBoxInStanupROI(const Rect2d& box)
{
	int index = -1;
	double center_x = box.x + box.width / 2;
	for (int i = 0; i < detectROI.faceROI.size(); ++i){
		double x1 = detectROI.faceROI[i].x - x_padding_standup*detectROI.faceROI[i].width;
		double x2 = detectROI.faceROI[i].x + detectROI.faceROI[i].width + x_padding_standup*detectROI.faceROI[i].width;
		if (center_x > x1 && center_x < x2){
			index = i;
			break;
		}
	}
	return index;
}

int ActionRecognition::_isBoxInStanupROI(const Rect2d& box, const double threshold)
{
	double maxOverlap = 0;
	int index = -1;
	for (int i = 0; i < detectROI.faceROI.size(); ++i){
		double overlap = _overlap(box, detectROI.faceROI[i]);
		if (overlap > maxOverlap){
			maxOverlap = overlap;
			index = i;
		}
	}
	if (maxOverlap > threshold)
		return index;
	else
		return -1;
}

double ActionRecognition::_overlap(Rect2d box1, Rect2d box2)
{
	int x1 = max(box1.x, box2.x);
	int y1 = max(box1.y, box2.y);
	int x2 = min(box1.x + box1.width, box2.x + box2.width);
	int y2 = min(box1.y + box1.height, box2.y + box2.height);
	int w = max(0, x2 - x1 + 1);
	int h = max(0, y2 - y1 + 1);
	int inter = w * h;
	int all = box1.width * box1.height + box2.width * box2.height - inter;
	return (double)inter / (double)all;
}

int ActionRecognition::_getStandupROI_index(Rect2d& box)
{
    int index = -1;
    double center_x = box.x + box.width/2;

	for (int i = 0; i < detectROI.faceROI.size(); i++){
		double x1 = detectROI.faceROI[i].x - x_padding_sitdown*detectROI.faceROI[i].width;
		double x2 = detectROI.faceROI[i].x + detectROI.faceROI[i].width + x_padding_sitdown*detectROI.faceROI[i].width;

		if (center_x > x1 && center_x < x2){
			index = i;
			break;
		}
	}
    return index;
}
