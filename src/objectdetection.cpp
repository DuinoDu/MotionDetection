#include "../include/objectdetection.h"


ObjectDetection::ObjectDetection()
{
    faceDetector = new FaceDetector;
}

ObjectDetection::~ObjectDetection()
{
    delete faceDetector;
}

void ObjectDetection::detect(Mat& frame, vector<Rect>& proposals, vector<vector<int>>& faces_box)
{
    faces_box.clear();
	for (auto rect : proposals){
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
	// << "before filter:" << faces_box.size() << endl;
	_filter(faces_box);
	//cout << "after filter:" << faces_box.size() << endl;
}

void ObjectDetection::setCaffemodelPath(string path)
{
    model_path = path;
	faceDetector->initialize(model_path); // , t1, t2, t3, minsize);
}

// useless??
void ObjectDetection::_filter(vector<vector<int>>& faces_box)
{
	vector<vector<int>> new_faces_box;
	for (auto box : faces_box){
		int center_x = (box[0] + box[2]) / 2;
		int center_y = (box[1] + box[3]) / 2;
		bool flag = false;
		// if box is in other box
		for (auto other_box : faces_box){
			if (center_x == (other_box[0] + other_box[2]) / 2 && center_y == (other_box[1] + other_box[3]) / 2){
				continue;
			}
			else{
				if (center_x > other_box[0] && center_x < other_box[2] && center_y > other_box[1] && center_y < other_box[3]){
					flag = true;
					break;
				}
			}
		}
		if (!flag)
			new_faces_box.push_back(box);
	}
	faces_box = new_faces_box;
}
