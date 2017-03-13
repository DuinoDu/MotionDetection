#include "../include/motiondetection.h"

#include "../include/log.h"

MotionDetection::MotionDetection()
{
    proposal = new MotionProposal;
    faceDetection = new ObjectDetection;
    faceTrack = new ObjectTrackVertical;
    //faceTrack = new ObjectTrack;
    faceTrack->setType("KCF");
    recognition = new ActionRecognition;
}

MotionDetection::~MotionDetection()
{
    delete proposal;
    delete faceDetection;
    delete faceTrack;
    delete recognition;
}

void MotionDetection::setParam(string paramName, float value)
{
    if (paramName == "RoomWidth"){
	}
    if (paramName == "RoomHeight"){
	}
	if (paramName == "standup_distance"){
		recognition->setStandup_t((double)value);
	}
	if (paramName == "sitdown_distance"){
		recognition->setSitdown_t((double)value);
	}
}
void MotionDetection::setParam(string paramName, string value)
{
    if (paramName == "CaffemodelPath"){
        faceDetection->setCaffemodelPath(value);
    }
}
void MotionDetection::setParam(string paramName, vector<int> &value)
{
    if(paramName == "StudentRegion"){
            assert(value.size() == 8);
            vector<Point2d> region;
            for(int i = 0; i < 4; ++i){
			    double x = value[2*i];   // I am a shabi.
                double y = value[2*i+1];
                region.push_back(Point2d(x, y));
            }
            proposal->setStudentRegion(region);
    }
}
void MotionDetection::showWindows(bool flag1, bool flag2)
{
    proposal->showWindow = flag1;
    this->showWindow = flag2;
}
void MotionDetection::setCallback(MyFun _myFun, void *_context)
{
    context  = _context;
    returnResult  =_myFun;
}
void MotionDetection::setFrame(Mat& _frame)
{
    CV_UNUSED(_frame);
}
void MotionDetection::setFrame(uchar* data, int height, int width, int length)
{
    assert(length/(height*width) == 3);
	Mat frame(height, width, CV_8UC3, (void*)data);
	detect(frame);
}

void MotionDetection::detect(Mat& frame)
{
    if (frame.isContinuous() && !frame.empty()){
        vector<vector<int>>  result;

        Mat frame_gray;
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

        // get motion regions
		vector<Rect> proposals;
		proposal->getProposal(frame_gray, proposals);
        //cout << "Contour size:" << contours.size() << endl;

		if (proposals.size() > 0){
			vector<Rect> new_proposals;
			for (auto rect : proposals){
			   if (_isTrackingObject(rect, faceTrack->getBoxes()))  // if need to remove contours that contain tracking objects???
				   continue;
			   if (rect.width < minProposal_w || rect.width > maxProposal_w 
				   || rect.height < minProposal_h || rect.height > maxProposal_h)
				   continue;
			   new_proposals.push_back(rect);
            }
			proposals = new_proposals;

			// detect and track face
			vector<vector<int>> faceBoxes;
            faceDetection->detect(frame, proposals, faceBoxes);
            faceTrack->update(frame, faceBoxes);
			recognition->detect(frame, faceTrack, faceDetection->get_faceDetector());
			
			for (auto box : recognition->detectROI.faceROI) {
                result.push_back({(int)(box.x + box.width/2), (int)(box.y + box.height/2) });  // cost much time
            }	
        }
        _sendResult(result);

		if (showWindow){
            // before faceDetection
			for (auto rect : proposals){
                cv::rectangle(frame, rect, Scalar(65, 255, 255), 2); 
            }

            // after faceDetection
            //for (auto box : faceBoxes) rectangle(frame, Rect2d(box[0], box[1], box[2] - box[0], box[3] - box[1]), Scalar(0, 0, 255), 2);

            // after faceTrack
            for (auto box : faceTrack->getBoxes()) rectangle(frame, box, Scalar(0, 0, 255), 2);

            // after recognition
            for (auto box : recognition->detectROI.faceROI) rectangle(frame, box, Scalar(255, 255, 0), 3);

            cv::imshow("img", frame);
        }
    }
}

void MotionDetection::_sendResult(const vector<vector<int>>& result)
{
    int *px = (int *)malloc(sizeof(int)* result.size());
    int *py = (int *)malloc(sizeof(int)* result.size());
    for (size_t i = 0; i < result.size(); ++i){
        *px = result[i][0];
        *py = result[i][1];
        ++px;
        ++py;
    }
    px -= result.size();
    py -= result.size();
    returnResult(px, py, result.size(), context);
    if (px != NULL) free(px);
    if (py != NULL) free(py);
}
bool MotionDetection::_isTrackingObject(const cv::Rect2d &rect, const vector<cv::Rect2d> &objects)
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
