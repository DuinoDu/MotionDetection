#include "motionclassification.h"

MotionClassification::MotionClassification()
{
    faceDetection = new ObjectDetection;
    faceTrack = new ObjectTrack;
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

        TickMeter time;

        time.start();
        faceDetection->detect(frame, contours, faceBoxes);
        time.stop(); if(showWindow) cout << "detect time:" << time.getTimeMilli() << "ms" << endl; time.reset();

        time.start();
        faceTrack->update(frame, faceBoxes);
        time.stop(); if(showWindow) cout << "track time:" << time.getTimeMilli() << "ms" << endl; time.reset();

        time.start();
        recognition->detect(faceTrack);
        time.stop(); if(showWindow) cout << "recognition time:" << time.getTimeMilli() << "ms" << endl; time.reset();

        for (auto box : recognition->getDetectROI().standupROI) {
            result.push_back({(int)(box.x + box.width/2), (int)(box.y + box.height/2) });
        }

        if (showWindow){
            // before faceDetection
            drawContours(frame, contours, -1, Scalar(255,255,0), 1);

            // after faceDetection
            for (auto box : faceBoxes) rectangle(frame, Rect2d(box[0], box[1], box[2]-box[0], box[3]-box[1]), Scalar(0,0,255), 2);

            // after faceTrack
            for (auto box : faceTrack->getBoxes()) rectangle(frame, box, Scalar(0,0,255), 2);

            // after recognition
            for (auto box : recognition->getDetectROI().standupROI) rectangle(frame, box, Scalar(255, 255, 0), 3);

            cv::imshow("img", frame);
        }
    }
}

void MotionClassification::setCaffemodelPath(string path)
{
    faceDetection->setCaffemodelPath(path);
}
