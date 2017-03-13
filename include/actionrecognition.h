#ifndef ACTIONRECOGNITION_H
#define ACTIONRECOGNITION_H

#include "common.h"
#include "objecttrack.h"
#include "./mtcnn/mtcnn.h"


struct DetectROI{
    vector<Rect2d> faceROI;
};

class ActionRecognition
{
public:
    ActionRecognition();
	void detect(const Mat& frame, ObjectTrack *tracker, FaceDetector* faceDetector);
	DetectROI detectROI;
	void setStandup_t(double param){ standup_t = param; }
	void setSitdown_t(double param){ sitdown_t = param; }

private:
    

    int _getStandupDisTh(Rect2d box);
    int _getSitdownDisTh(Rect2d box);

	int _isBoxInStanupROI(const Rect2d& box);
	int _isBoxInStanupROI(const Rect2d& box, const double threshold);
	double _overlap(Rect2d box1, Rect2d box2);
    int _getStandupROI_index(Rect2d& box);
	int frame_cnt = 0;

	// parameter
	double x_padding_standup = 0.5;
	double x_padding_sitdown = 1;
	double standup_t = 1;
    double sitdown_t = 0.5;
	int check_period = 8;  // check if face exists in region
	double padding_while_standing_x = 10;
	double padding_while_standing_y = 10;
	int max_standup = 2;
	
};

#endif // ACTIONRECOGNITION_H
