#include "../include/objectdetection.h"
#include "mtcnn/mtcnn.h"

void ObjectDetection::read_basic(const FileNode& fn)
{
    cout << "read ObjectDetection config" << endl;
}

void ObjectDetection::filter(vector<vector<int> >& object_box)
{
	vector<vector<int>> new_box;
	for (auto box : object_box){
		int center_x = (box[0] + box[2]) / 2;
		int center_y = (box[1] + box[3]) / 2;
		bool flag = false;
		// if box is in other box
		for (auto other_box : object_box){
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
			new_box.push_back(box);
	}
	object_box = new_box;
}

class FaceDetection : public ObjectDetection
{
public:
    FaceDetection();
    ~FaceDetection();
    virtual void read(const FileNode& fn);
    virtual void detect(const Mat& input, vector<Rect>& proposals, vector<vector<int> >& object_box);
    virtual void setCaffeModelPath(const string& path);

private:
    FaceDetector *faceDetector;
    /* parameter */
    string modelPath;
    float t1 = 0.6;
    float t2 = 0.7;
    float t3 = 0.7;
    float minsize = 10;
};

FaceDetection::FaceDetection()
{
    faceDetector = new FaceDetector;
}

FaceDetection::~FaceDetection()
{
    delete faceDetector;
}

void FaceDetection::read(const FileNode& fn)
{
    read_basic(fn);
    cout << "read FaceDetection config" << endl;
    t1 = (float)fn["mtcnn_t1"];
    t2 = (float)fn["mtcnn_t2"];
    t3 = (float)fn["mtcnn_t3"];
    minsize = (float)fn["mtcnn_minsize"];
	if (modelPath != ""){
		faceDetector->initialize(modelPath); // , t1, t2, t3, minsize);
	}
}

void FaceDetection::setCaffeModelPath(const string& path)
{
    modelPath = path;
    faceDetector->initialize(modelPath); // , t1, t2, t3, minsize);
}

void FaceDetection::detect(const Mat& input, vector<Rect>& proposals, vector<vector<int> >& object_box)
{
    CV_Assert(!input.empty());
    CV_Assert(input.type() == CV_8UC3);
    object_box.clear();
    for (auto rect : proposals){
        Mat roi = input.rowRange(rect.y, rect.y + rect.height).colRange(rect.x, rect.x + rect.width);
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
            object_box.push_back(i);
        }
    }
    filter(object_box);
}

Ptr<ObjectDetection> createObjectDetection()
{
    return makePtr<FaceDetection>();
}
