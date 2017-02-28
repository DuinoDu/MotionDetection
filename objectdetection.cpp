#include "objectdetection.h"

ObjectDetection::ObjectDetection()
{
    faceDetector = new FaceDetector;
}

ObjectDetection::~ObjectDetection()
{
    delete faceDetector;
}

void ObjectDetection::detect(Mat& frame, vector<vector<Point>>& contours, vector<vector<int>>& faces_box)
{
    faces_box.clear();
    for(auto c : contours){
        Rect rect;
        rect = boundingRect(c);
        Mat roi = frame.rowRange(rect.y, rect.y + rect.height).colRange(rect.x, rect.x + rect.width);

        vector<vector<int>> face_box;
        faceDetector->detect(roi, face_box);
        if(face_box.empty()){
            continue;
        }
        else{
            for(int i = 0; i < face_box.size(); ++i){
                face_box[i][0] += rect.x;
                face_box[i][1] += rect.y;
                face_box[i][2] += rect.x;
                face_box[i][3] += rect.y;
            }
        }
        for ( auto i : face_box){
            faces_box.push_back(i);
        }
    }
}

void ObjectDetection::setCaffemodelPath(string path)
{
    model_path = path;
    faceDetector->initialize(model_path);
}
