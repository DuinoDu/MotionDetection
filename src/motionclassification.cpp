#include "../include/motionclassification.h"
#include "../include/log.h"

MotionClassification::MotionClassification()
{
    faceDetection = new ObjectDetection;
    faceTrack = new ObjectTrackVertical;
    faceTrack->setType("KCF");
    recognition = new ActionRecognition;
}

MotionClassification::~MotionClassification()
{
    delete faceDetection;
    delete faceTrack;
    delete recognition;
}

void MotionClassification::classify(cv::Mat &frame, vector<vector<cv::Point> > contours, vector<vector<int>>& result)
{
    if (contours.size() > 0){ 
        faceBoxes.clear();

		vector<Rect> proposals;
		for (auto c : contours){
			Rect rect;
			rect = boundingRect(c);
			if (!_isTrackingObject(rect, faceTrack->getBoxes()))  // if need to remove contours that contain tracking objects???
				proposals.push_back(rect);
		}

        TickMeter time;

        time.start();
		faceDetection->detect(frame, proposals, faceBoxes);
        time.stop(); //if(showWindow) cout << "detect time:" << time.getTimeMilli() << "ms" << endl; time.reset();

        time.start();
        faceTrack->update(frame, faceBoxes);
        time.stop(); //if(showWindow) cout << "track time:" << time.getTimeMilli() << "ms" << endl; time.reset();

        time.start();
        recognition->detect(faceTrack);
        time.stop(); //if(showWindow) cout << "recognition time:" << time.getTimeMilli() << "ms" << endl; time.reset();

		for (auto box : recognition->getDetectROI().faceROI) {
            result.push_back({(int)(box.x + box.width/2), (int)(box.y + box.height/2) });
        }
    }

	if (showWindow){
		// before faceDetection
		//drawContours(frame, contours, -1, Scalar(255, 255, 0), 3);
		for (auto c : contours){
			Rect rect;
			rect = boundingRect(c);
			cv::rectangle(frame, rect, Scalar(255, 100, 100), 2);
		}

		// after faceDetection
		//for (auto box : faceBoxes) rectangle(frame, Rect2d(box[0], box[1], box[2] - box[0], box[3] - box[1]), Scalar(0, 0, 255), 2);

		// after faceTrack
		for (auto box : faceTrack->getBoxes()) rectangle(frame, box, Scalar(0, 0, 255), 2);

		// after recognition
		for (auto box : recognition->getDetectROI().faceROI) rectangle(frame, box, Scalar(255, 255, 0), 3);

		cv::imshow("img", frame);
	}
}

void MotionClassification::setCaffemodelPath(string path)
{
    faceDetection->setCaffemodelPath(path);
}

bool MotionClassification::_isTrackingObject(const Rect2d& rect, const vector<Rect2d>& objects)
{
	bool flag = false;
	for (auto object : objects){
		double x1 = max(rect.x, object.x);
		double y1 = max(rect.y, object.y);
		double x2 = min(rect.x + rect.width, object.x + object.width);
		double y2 = min(rect.y + rect.height, object.y + object.height);
		double w = max(0, x2 - x1 + 1);
		double h = max(0, y2 - y1 + 1);
		double inter = w * h;
		double overlap = inter / (rect.width*rect.height + object.width*object.height - inter);
		if (overlap > 0.5){
			flag = true;
		}
	}
	return flag;
}
