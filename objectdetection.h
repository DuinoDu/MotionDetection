#ifndef OBJECTDETECTION_H
#define OBJECTDETECTION_H

#include "common.h"
#include "../mtcnn/mtcnn.h"

class ObjectDetection
{
public:
    ObjectDetection();
    ~ObjectDetection();
    void detect(Mat& frame, vector<vector<Point>>& contours, vector<vector<int>>& faces_box);
    void setCaffemodelPath(string path);

private:
    string model_path;
    FaceDetector *faceDetector;
};

#endif // OBJECTDETECTION_H
