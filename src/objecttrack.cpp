#include "../include/objecttrack.h"

/* ObjectTrack */

void ObjectTrack::read_basic(const FileNode& fn)
{
    cout << "read ObjectTrack config" << endl;
    maxObjects = (int)fn["maxObjects"];
}

/* ObjectTrackVertical */

class ObjectTrackVertical : public ObjectTrack
{
public:
    virtual void read(const FileNode& fn);
    virtual void update(const Mat& img, const vector<vector<int>>& boxes);
private:
    void convert(const vector<vector<int>>&, vector<Rect2d>&);
    void filterObjects(vector<Rect2d>);
    int findPair(const Rect2d&, const vector<Rect2d>&);
    bool isPair(const Rect2d&, const Rect2d&);

    /* parameters */
    double x_paddingscale_left = 0.3;  // scale
    double x_paddingscale_right = 0.3;
    int timelength = 5;
};

void ObjectTrackVertical::read(const FileNode& fn)
{
    read_basic(fn);
    cout << "read ObjectTrackVertical config" << endl;
    x_paddingscale_left = (double)fn["x_paddingscale_left"];
    x_paddingscale_right = (double)fn["x_paddingscale_right"];
    timelength = (int)fn["timelength"];
}

void ObjectTrackVertical::update(const Mat& img, const vector<vector<int>>& boxes)
{
    CV_UNUSED(img);
    vector<Rect2d> boxesRect;
    convert(boxes, boxesRect);
    filterObjects(boxesRect);

    assert(objects.size() == movements.size());
    assert(objects.size() == times.size());

    for (size_t i = 0; i < objects.size(); i++){
        // 1. update moving objects
        int index = -1;
        index = findPair(objects[i], boxesRect);
        assert(index >= -1 && index < (int)boxesRect.size());
        if (index > -1){
			double delta = (boxesRect[index].y + boxesRect[index].height / 2) - (objects[i].y + objects[i].height / 2);
			if (delta < 0){
				movements[i] += delta; //
				objects[i].x = boxesRect[index].x;
				objects[i].y = boxesRect[index].y;
				objects[i].width = boxesRect[index].width;
				objects[i].height = boxesRect[index].height;
				times[i] = timelength;
			}
        }

        // 2. remove still objects
        times[i] = (times[i] > 0) ? times[i]-1 : 0 ;
        if (times[i] == 0){
            movements.erase(movements.begin() + i);
            objects.erase(objects.begin() + i);
            times.erase(times.begin() + i);
        }
    }
    // 3. add new object
    for (size_t i = 0; i < boxesRect.size(); i++){
        if (objects.size() > maxObjects)
            break;
        int index_new = -1;
        index_new = findPair(boxesRect[i], objects);
        assert(index_new >= -1);
        if (index_new == -1){
            objects.push_back(boxesRect[i]);
            movements.push_back(0);
            times.push_back(timelength);
        }
    }
}

void ObjectTrackVertical::convert(const vector<vector<int>>& _boxes, vector<Rect2d>& _objects)
{
    for (auto box : _boxes){
        Rect2d newObject((double)box[0], (double)box[1], (double)(box[2] - box[0]), (double)(box[3] - box[1]));
        _objects.push_back(newObject);
    }
}

void ObjectTrackVertical::filterObjects(vector<Rect2d> _objects)
{
    CV_UNUSED(_objects);
    // ???
}

int ObjectTrackVertical::findPair(const Rect2d& box, const vector<Rect2d>& boxes)
{
    int index = -1;
    for (size_t i = 0; i < boxes.size(); i++){
        if (isPair(box, boxes[i])){
            index = i;
            break;
        }
    }
    return index;
}

bool ObjectTrackVertical::isPair(const Rect2d& rect1, const Rect2d& rect2)
{
    double x1 = rect1.x - x_paddingscale_left * rect1.width;
    double x2 = rect1.x + rect1.width + x_paddingscale_right * rect1.width;
    return ((rect2.x + rect2.width / 2) >= x1 &&
            (rect2.x + rect2.width / 2) <= x2);
}

Ptr<ObjectTrack> createObjectTrack()
{
    return makePtr<ObjectTrackVertical>();
}
