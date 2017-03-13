#ifndef OBJECTDETECTION_H
#define OBJECTDETECTION_H

#include "common.h"
#include "./mtcnn/mtcnn.h"

class ObjectDetection
{
public:
    ObjectDetection();
    ~ObjectDetection();
	void detect(Mat& frame, vector<Rect>& proposals, vector<vector<int>>& faces_box);
    void setCaffemodelPath(string path);
	FaceDetector* get_faceDetector(){ return faceDetector; }

private:
    string model_path;
    FaceDetector *faceDetector;
	void _filter(vector<vector<int>>& faces_box);

	// parameter
	float t1 = 0.6;
	float t2 = 0.7;
	float t3 = 0.7;
	float minsize = 10;
};

#endif // OBJECTDETECTION_H
