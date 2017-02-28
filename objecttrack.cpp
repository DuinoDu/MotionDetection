#include "objecttrack.h"

#include <iostream>
using std::cout;
using std::endl;

ObjectTrack::ObjectTrack()
{
}

ObjectTrack::~ObjectTrack()
{
    delete multiTracker;
}

void ObjectTrack::setType(const string& type)
{
    multiTracker = new cv::MultiTracker(type);
}

void ObjectTrack::update(const Mat& img, const vector<vector<int>>& boxes)
{
    // 1. update old box position---movements, times
    // 2. delete still boxes
    // 3. add new box

    if(multiTracker->objects.size() > 0){
        multiTracker->update(img);
        _updateMovements();
        _updateTimes();
    }
    _removeStillBox();
    for(auto box : boxes){
        _addBox(img, box);
    }
    _updateObjects();
}

int ObjectTrack::getObjectNum()
{
    return objects.size();
}

vector<Rect2d> ObjectTrack::getBoxes()
{
    return multiTracker->objects;
}

vector<double> ObjectTrack::getMovements()
{
    return movements;
}

void ObjectTrack::_addBox(const Mat& img, const vector<int>& new_box)
{
    assert(img.isContinuous());
    assert(new_box.size() == 4);
    if(!_isOldBox(new_box)){
        Rect2d newObject((double)new_box[0], (double)new_box[1], (double)(new_box[2]-new_box[0]), (double)(new_box[3]-new_box[1]));
        objects.push_back(newObject);
        Point2d center((new_box[0] + new_box[2])/2, (new_box[1] + new_box[3])/2);
        origins.push_back(center);
        movements.push_back(0);
        times.push_back(stillImageTime);
        multiTracker->add(img, newObject);
    }
}

void ObjectTrack::_updateMovements()
{
    assert(objects.size() == multiTracker->objects.size());
    assert(objects.size() == movements.size());
    for(int i = 0; i < movements.size(); ++i){
        double old_y = objects[i].y + objects[i].height/2;
        double new_y = multiTracker->objects[i].y + multiTracker->objects[i].height/2;
        movements[i] += new_y - old_y;
    }
}

void ObjectTrack::_updateTimes()
{
    assert(objects.size() == multiTracker->objects.size());
    assert(objects.size() == times.size());
    for(int i = 0; i < times.size(); ++i){
        double old_x = objects[i].x + objects[i].width/2;
        double new_x = multiTracker->objects[i].x + multiTracker->objects[i].width/2;
        double old_y = objects[i].y + objects[i].height/2;
        double new_y = multiTracker->objects[i].y + multiTracker->objects[i].height/2;
        double distance = std::abs(old_x - new_x) + std::abs(old_y - new_y);
        //cout << "distance: " << distance << endl;
        if(distance < stillDisThreshold){
            times[i]--;
        }
        else{
            times[i] = stillImageTime;
        }
    }
}

void ObjectTrack::_updateObjects()
{
    objects = multiTracker->objects;
}

void ObjectTrack::_removeStillBox()
{
    assert(objects.size() == multiTracker->objects.size());
    assert(objects.size() == movements.size());
    assert(objects.size() == times.size());
    assert(objects.size() == origins.size());
    for(int i = 0; i < objects.size(); ++i){
        if(times[i] < 0){
            multiTracker->objects.erase(multiTracker->objects.begin() + i);
            multiTracker->remove(i);
            //multiTracker->trackerList.erase(multiTracker->trackerList.begin() + i);
            objects.erase(objects.begin() + i);
            origins.erase(origins.begin() + i);
            movements.erase(movements.begin() + i);
            times.erase(times.begin() + i);
        }
    }

}

bool ObjectTrack::_isOldBox(vector<int> box)
{
    Point2d center((box[0] + box[2])/2, (box[1] + box[3])/2);
    Point2d p1(box[0], box[1]);
    Point2d p2(box[0], box[3]);
    Point2d p3(box[2], box[1]);
    Point2d p4(box[2], box[3]);

    for(auto object : objects){
        if(object.contains(p1) ||
           object.contains(p2) ||
           object.contains(p3) ||
           object.contains(p4) ||
           object.contains(center)){
            return true;
        }
    }
    return false;
}

void ObjectTrack::add_test(const Mat& img, const vector<int>& box)
{
    _addBox(img, box);
}
void ObjectTrack::update_test(const Mat& img)
{
    multiTracker->update(img);
}
Rect2d ObjectTrack::getCurrentBox_test()
{
    return multiTracker->objects.back();
}
